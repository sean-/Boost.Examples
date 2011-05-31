#ifndef PIMPL_GENERALIZATION_HEADER_VB
#define PIMPL_GENERALIZATION_HEADER_VB

// Copyright (c) 2001 Peter Dimov and Multi Media Ltd.
// Copyright (c) 2006-2008 Vladimir Batov.
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. See http://www.boost.org/LICENSE_1_0.txt.

#include "./detail/pimpl_constructors.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/type_traits.hpp>
#include <boost/serialization/shared_ptr.hpp>

// a) The __DEPLOY_IF_NOT_PIMPL_DERIVED__ macro makes sure that the 1-arg
//    constructor is not called when the copy constructor is in order.
// b) The macro uses boost::is_base_of<base, A> instead of pimpl<>::value as we
//    need to check if A is derived from 'base'. If 'no', the macro enables call
//    the respective constructor. If 'yes', then it disables that constructor as
//    the base copy constructor needs to be called instead.
// c) The macro uses the 'internal_type' type to uniquely distinguish the
//    respective constructor from ANY of 2-args constructors.
#undef  __DEPLOY_IF_NOT_PIMPL_DERIVED__
#define __DEPLOY_IF_NOT_PIMPL_DERIVED__(A)  \
    typename boost::disable_if<             \
        boost::is_base_of<base, A>, internal_type*>::type =0

template<class T>
struct pimpl /// Generalization of the Pimpl idiom
{
    struct implementation;
    template<class> class impl_ptr;
    template<template<class> class> class pimpl_base;
    /// Convenience typedef to deploy pimpl with value semantics.
    typedef pimpl_base<impl_ptr> value_semantics;
    /// Convenience typedef to deploy pimpl with pointer semantics.
    typedef pimpl_base<boost::shared_ptr> pointer_semantics;
};

/// Type-traits functionality for the pimpl.
/// is_pimpl<Foo>::value is 'true' if Foo directly or indirectly inherits from
/// pimpl<>. Otherwise, is_pimpl<Foo>::value is 'false'.
template<class T>
struct is_pimpl
{
    typedef ::boost::type_traits::yes_type yes_type;
    typedef ::boost::type_traits::no_type no_type;
    typedef typename boost::remove_reference<T>::type* ptr_type;

    template<class Y>
    static yes_type tester (Y const*, typename Y::is_pimpl_base const* =0);
    static no_type  tester (...);

    BOOST_STATIC_CONSTANT(bool, value = (1 == sizeof(tester(ptr_type(0)))));
};

/// Smart-pointer with value-semantics behavior.
/// It complements boost::shared_ptr which takes care of pointer-semantics
/// behavior. The incomplete-type management technique is by Peter Dimov as
/// originally implemented in boost::impl_ptr.
template<class Interface>
template<class Impl/*ementation*/>
class pimpl<Interface>::impl_ptr
{
    public:

   ~impl_ptr () { traits_->destroy(impl_); }
    impl_ptr () : traits_(deep_copy()), impl_(0) {}
    impl_ptr (Impl* p)
    : traits_(deep_copy()), impl_(p) {}
    impl_ptr (impl_ptr const& that)
    : traits_(that.traits_), impl_(traits_->copy(that.impl_)) {}
    impl_ptr& operator=(impl_ptr const& that)
    {
        traits_->assign(impl_, that.impl_);
        return *this;
    }
    bool operator<(impl_ptr const& that) const
    {
        return this->impl_ < that.impl_;
        // Should it be "*this->impl_ < *that.impl_" instead?
    }
    void reset(Impl* p) { impl_ptr(p).swap(*this); }

    void swap(impl_ptr& that)
    {
        std::swap(impl_,   that.impl_);
        std::swap(traits_, that.traits_);
    }
    Impl* get() { return impl_; }
    Impl const* get() const { return impl_; }

    private:

    struct traits
    {
        virtual ~traits() {}
        virtual void destroy (Impl*&) const =0;
        virtual Impl*   copy (Impl const*) const =0;
        virtual void  assign (Impl*&, Impl const*) const =0;
    };
    struct deep_copy : public traits
    {
        virtual void destroy (Impl*& p) const { boost::checked_delete(p); p = 0; }
        virtual Impl*   copy (Impl const* p) const { return p ? new Impl(*p) : 0; }
        virtual void  assign (Impl*& a, Impl const* b) const
        {
            /**/ if ( a ==  b);
            else if ( a &&  b) *a = *b;
            else if (!a &&  b) a = copy(b);
            else if ( a && !b) destroy(a);
        }
        operator traits const*()
        {
            static deep_copy impl;
            return &impl;
        }
    };

    traits const* traits_;
    Impl*           impl_;
};

template<class T>
template<template<class> class Manage>
class pimpl<T>::pimpl_base /// Base class that all pimpl-based classes derive from
{
    public:

    /// Convenience typedef to access/manipulate implementation data.
    typedef typename pimpl<T>::implementation implementation;
    /// Convenience typedef to access/initialize this class in the derived
    /// classes.
    typedef pimpl_base<Manage> base;
    /// @name Support for Conversions To Boolean
    /// @details Implementing an implicit conversion to bool (operator bool()
    /// const) is very convenient and tempting to be deployed in constructs like
    /// "if (pimpl)" and "if (!pimpl)" (without explicit op!()). However, sadly,
    /// implementing "operator bool()" is WRONG as that conversion kicks in way
    /// too often and unexpectedly. Like in "pimpl == 1", "pimpl+1", "1+pimpl"
    /// or potentially during boost::lexical_cast<string>(pimpl) (if there are
    /// no op>>() and op<<() defined). Consequently, that "implicit conversion
    /// to bool" functionality has to be implemented in an indirect and somewhat
    /// awkward way via an implicit conversion to some other type. The best type
    /// for the purpose appears to be a pointer to a member function.\n\n
    /// For more see the chapter 7.7 in Alexandrescu's "Modern C++ Design" and
    /// how that conversion-to-bool is implemented for boost::shared_ptr.
    //@{
    typedef void (base::*bool_type)();
    void internal_bool() {};
    operator bool_type() const { return impl_.get() ? &base::internal_bool : 0; }
    //@}

    /// @name Invalid Pimpl with NULL Pointer-Like Behavior
    //@{
    /// The function returns an invalid NULL-like instance of the T type (that
    /// is immediately derived from pimpl<T>). Consequently, it does not work
    /// with class hierarchies (like Base and Derived) because Derived::null()
    /// will still return an instance of Base. To get a null instance of the
    /// Derived type null<Derived>() needs to be called instead.
    /// One of the usages is to disable automatic interface-implementation
    /// management as in
    /// @code
    ///     Foo::Foo(parameters) : base(null()) {...}
    /// @endcode
    /// or to return an invalid NULL-like Foo instance with
    /// @code
    ///     return Foo::null();
    /// @endcode
    static T null() { return null<T>(); }

    /// Return an invalid NULL-like instance of the Derived type derived from
    /// pimpl<T>. It is to be used for class hierarchies (Foo::null() is better
    /// looking but only works with the classes directly derived from pimpl<>).
    /// Function is protected by 'enable_if' to make sure Derived inherits from
    /// pimpl<T> (directly or indirectly).
    template<class Derived>
    static
    typename boost::enable_if<is_pimpl<Derived>, Derived>::type
    null()
    {
        internal_type null_value;
        typename Derived::base null_instance(null_value);
        return *(Derived*) &null_instance;
    }
    //@}
    /// @name Comparison Operators
    //@{
    /// @details pimpl_base::op==() simply transfers the comparison down to its
    /// implementation policy -- boost::shared_ptr or pimpl::impl_ptr.
    /// Consequently, a pointer-semantics (shared_ptr-based) pimpls are
    /// comparable as there is shared_ptr::op==(). However, a value-semantics
    /// (pimpl::impl_ptr-based) pimpl is not comparable by default (the standard
    /// value-semantics behavior) as pimpl::impl_ptr::op==() is not implemented.
    /// If a value-semantics class T needs to be comparable, then it has to
    /// *explicitly* provide T::op==(T const&) as part of its public interface.
    /// Trying to call this pimpl_base::op==() for impl_ptr-based pimpl will
    /// fail to compile (no impl_ptr::op==()) and will indicate that the user
    /// forgot to declare T::operator==(T const&).
    bool operator==(T const& that) const { return impl_ == that.impl_; }
    /// For pimpl to be used in std associative containers.
    bool operator<(base const& that) const { return this->impl_ < that.impl_; }
    //@}

    /// Swap the underlying content of two objects
    void swap(T& that) { impl_.swap(that.impl_); }

    private:

    // Internal type for internal use.
    struct internal_type {};

    // Creates an invalid (with no implementation) NULL-like instance.
    // Used internally and solely by pimpl::null().
    pimpl_base (internal_type) {}

    protected:

    /// @details Destructor is empty but is defined explicitly as 'protected' to
    /// make sure that pimpl<>::pimpl_base cannot be used directly.
   ~pimpl_base() {}

    // The default auto-generated copy constructor and the default assignment
    // operator are just fine (do member-wise copy and assignment respectively).

    /// @name Forwarding Constructors
    /// @details A series of forwarding constructors. These constructors
    /// dutifully transfer arguments to the corresponding constructors of the
    /// internal 'implementation' class. That is done to encapsulate the
    /// construction of the 'implementation' instance inside this class and,
    /// consequently, to *fully* automate memory management (rather than just
    /// deletion).
    //@{
    pimpl_base() : impl_(new implementation()) {}
	__MANY_MORE_PIMPL_CONSTRUCTORS__;
    //@}
    /// @name Access To Implementation
    /// @details Functions are protected. They only allow access to underlying
    /// data for the derived classes and only inside implementation files (where
    /// pimpl<>::implementation is visible) in any of the following:
    /// @code
    ///      implementation* impl = *this;
    ///      implementation const* impl = *this; // inside 'const' methods
    ///      implementation& impl = **this;
    ///      implementation const& impl = **this; // inside 'const' methods
    /// @endcode
    /// then implementation data are accessed/manipulated as
    /// @code
    ///      implementation& impl = **this;
    ///      impl.data = ...;
    /// @endcode
    /// The Pimpl class does NOT behave like a pointer, where, say, 'const
    /// shared_ptr' allows to still modify the underlying data. Instead, the
    /// Pimpl class behaves more like a proxy. Therefore, 'const' instances
    /// return 'const' pointers and references.
    //@{
    implementation const* operator->() const { BOOST_ASSERT(impl_.get()); return  impl_.get(); }
    implementation const& operator *() const { BOOST_ASSERT(impl_.get()); return *impl_.get(); }
    implementation*       operator->()       { BOOST_ASSERT(impl_.get()); return  impl_.get(); }
    implementation&       operator *()       { BOOST_ASSERT(impl_.get()); return *impl_.get(); }
    //@}

    /// @name Explicit Management of Interface-Implementation Associations
    /// @details Explicitly set/reset the internal interface-implementation
    /// association to point to the supplied implementation. It is similar to
    /// std::auto_ptr::reset() or boost::shared_ptr::reset(). It is deployed
    /// when the default management of the interface-implementation associations
    /// does not suffice. Like lazy-instantiation optimization or pimpl<> use in
    /// run-time polymorphic hierarchies.
    //@{
    void reset(implementation* impl) { impl_.reset(impl); }
    template<class Y, class D> void reset(Y* p, D d) { impl_.reset(p, d); }
    //@}

    private:

    template<class Y> friend struct pimpl<Y>::implementation;
    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive& a, unsigned int)
    {
        a & BOOST_SERIALIZATION_NVP(impl_);
    }

    // A internal typedef used in is_pimpl<>.
    typedef pimpl_base<Manage> is_pimpl_base;

    Manage<implementation> impl_;
};

/// A free-standing incarnation of the null() function that returns an invalid
/// NULL-like pimpl<>. As its T::null<T>() member-function cousin it is to be
/// used for class hierarchies. This function though can be called simply as
/// null<Foo>() outside Foo member functions as
/// @code
///     Foo bad = null<Foo>();
/// @endcode
template<class T>
inline
typename boost::enable_if<is_pimpl<T>, T>::type
null()
{
    return T::template null<T>();
}

// 1. Catch ALL pimpl-related comparison operators and handle them explicitly.
//    Otherwise, conversion to bool kicks in instead (say, for foo==5 or 5==foo)
//    which is most unlikely to be what is needed.
// 2. All op==() and op!=() are implemented via T::op==(T const&) so that the
//    user gets op!=() for free when T::op==() is defined.
// 3. The comparison behavior that appears to be the standard is that for, say,
//    "foo == (int) 5" to succeed, there has to be either Foo::operator==(int)
//    or Foo::operator int(). The same goes for "(int) 5 == foo": there has to
//    be available ::operator==(Foo) or an acceptable conversion. Non-explicit
//    Foo::Foo(int) is not considered to be a conversion.
//    For example, for "struct Foo { Foo(int) }" VS2007 gives
//    binary '==' : 'Foo' does not define this operator or a conversion to a
//                  type acceptable to the predefined operator
//    binary '==' : no global operator found which takes type 'Foo' (or there is
//                  no acceptable conversion)
//    for "foo == 5" and "5 == foo" respectively. GCC gives similar errors.
//    Therefore, we want Pimpl comparisons to behave the same way. Namely, if
//    Pimpl-based Foo defines Foo::operator==(int const&) explicitly, then it'll
//    be deployed. Otherwise, __IF_PIMPLA__(A,B) operator==() kicks in where we
//    try deploying Foo::operator int() and fail if such a conversion does not
//    exist.
#define __IF_PIMPLS__(A,B) template<class A, class B> inline typename boost::enable_if_c< is_pimpl<A>::value &&  is_pimpl<B>::value, bool>::type
#define __IF_PIMPLA__(A,B) template<class A, class B> inline typename boost::enable_if_c< is_pimpl<A>::value && !is_pimpl<B>::value, bool>::type
#define __IF_PIMPLB__(A,B) template<class A, class B> inline typename boost::enable_if_c<!is_pimpl<A>::value &&  is_pimpl<B>::value, bool>::type

__IF_PIMPLS__(A,B) operator==(A const& a, B const& b) { return  a.operator==(b); }
__IF_PIMPLS__(A,B) operator!=(A const& a, B const& b) { return !a.operator==(b); }
__IF_PIMPLA__(A,B) operator==(A const& a, B const& b) { return  (B(a) == b); }
__IF_PIMPLA__(A,B) operator!=(A const& a, B const& b) { return !(B(a) == b); }
__IF_PIMPLB__(A,B) operator==(A const& a, B const& b) { return  (a == A(b)); }
__IF_PIMPLB__(A,B) operator!=(A const& a, B const& b) { return !(a == A(b)); }

#undef __IF_PIMPLS__
#undef __IF_PIMPLA__
#undef __IF_PIMPLB__

#if defined(__linux__) && 1 < 0
#	include <boost/archive/binary_iarchive.hpp>
#	include <boost/archive/binary_oarchive.hpp>
#	include <boost/archive/text_iarchive.hpp>
#	include <boost/archive/text_oarchive.hpp>
#	include <boost/archive/xml_iarchive.hpp>
#	include <boost/archive/xml_oarchive.hpp>
#endif

// To deploy boost::serialization for a Pimpl-based class the following steps
// are needed:
// C1. Add the following serialization DECLARATION to the interface class
//     __CLASS__:
//          private:
//          friend class boost::serialization::access;
//          template<class Archive> void serialize(Archive&, unsigned int);
// C2. Add the following serialization DEFINITION to the
//     pimpl<__CLASS__>::implementation class
//          template<class Archive>
//          void
//          serialize(Archive& a, unsigned int file_version)
//          { THE SERIALIZATION OF YOUR PIMPL IMPLEMENTATION GOES HERE }
// C3. Add one of the BOOST_SERIALIZATION_PIMPL...(__CLASS__) macros to the same
//     file where pimpl<__CLASS__>::implementation is declared and implemented.
// C4. For more info see http://www.boost.org/libs/serialization/doc/index.html
//     and then go to "Case Studies/PIMPL"
// C5. BOOST_SERIALIZATION_PIMPL_REPLACE has the standard boost::serialization
//     behavior, i.e. it RESETS the supplied pimpl as the macro delegates
//     serialization to the underlying boost::shared_ptr which in turn REPLACES
//     the underlying implementation data with newly created and serialized
//     data. See boost/shared_ptr.hpp where load() looks like
//
//     void load(Archive& ar, boost::shared_ptr<T>& t, const unsigned int)
//     {
//         T* r;
//         ar >> boost::serialization::make_nvp("px", r);
//         get_helper<Archive, detail::shared_ptr_helper >(ar).reset(t,r);
//     }
//     All that is done for a reason. That allows boost::serialization to track
//     shared pointers and, therefore, to avoid duplicates.
// C6. Given the supplied pimpl content is replaced, there is no reason to
//     create that temporary pimpl (that by default requires the default
//     constructor. For that reason we initialize that replaceable pimpl with
//     null().
// C7. BOOST_SERIALIZATION_PIMPL_INPLACE works differently as it does not
//     serialize the underlying boost::shared_ptr but instead populates/loads
//     into the user-provided implementation data. That certainly lacks
//     boost::serialization standard object-tracking properties of
//     BOOST_SERIALIZATION_PIMPL_REPLACE as the serialization step of
//     boost::shared_ptr is by-passed. Consequently, it is not suitable when a
//     pimpl is restored from scratch or many pimpls pointing to the same data
//     are stored/restored. However, it is essential when an application
//     controls the creation of a pimpl. Say, a pimpl object is a singleton and
//     by the time we try restoring from the persistent state there are many
//     pimpls referencing to the same instance. Then, we need to replace the
//     content of the already existing implementation rather than replacing the
//     implementation instance as a whole.
//     Another application might be when an instance is initially constructed
//     manually (not restored from a persistent state) and, say, only partially
//     initialized from, say, (static) configuration data. Then more (dynamic)
//     data can be loaded via serialization into the EXISTING instance.
#define BOOST_SERIALIZATION_PIMPL_EXPLICIT_INSTANTIATIONS(__CLASS__)                    \
    /* Explicit instantiation of the serialization code. Add more when needed. */       \
    template void __CLASS__::serialize(boost::archive::binary_iarchive&, unsigned int); \
    template void __CLASS__::serialize(boost::archive::binary_oarchive&, unsigned int); \
    template void __CLASS__::serialize(boost::archive::  text_iarchive&, unsigned int); \
    template void __CLASS__::serialize(boost::archive::  text_oarchive&, unsigned int); \
    template void __CLASS__::serialize(boost::archive::   xml_iarchive&, unsigned int); \
    template void __CLASS__::serialize(boost::archive::   xml_oarchive&, unsigned int); \

#define BOOST_SERIALIZATION_PIMPL_REPLACE(__CLASS__) /* See C5 */                       \
                                                                                        \
    namespace boost                                                                     \
    {                                                                                   \
        namespace serialization                                                         \
        {                                                                               \
            template<class Archive>                                                     \
            inline                                                                      \
            typename boost::enable_if<is_pimpl<__CLASS__>, void>::type                  \
            load_construct_data(                                                        \
                Archive& ar,                                                            \
                __CLASS__* t,                                                           \
                const unsigned int file_version)                                        \
            {                                                                           \
                ::new(t) __CLASS__(null<__CLASS__>()); /* See C6 */                     \
            }                                                                           \
        }                                                                               \
    }                                                                                   \
    /* Serialization definition for __CLASS__ */                                        \
    template<class Archive>                                                             \
    void                                                                                \
    __CLASS__::serialize(Archive& a, unsigned int)                                      \
    {                                                                                   \
        a & BOOST_SERIALIZATION_BASE_OBJECT_NVP(base);                                  \
    }                                                                                   \
    BOOST_SERIALIZATION_PIMPL_EXPLICIT_INSTANTIATIONS(__CLASS__)                        \

#define BOOST_SERIALIZATION_PIMPL_INPLACE(__CLASS__) /* See C7 */                       \
                                                                                        \
    /* Serialization definition for __CLASS__ */                                        \
    template<class Archive>                                                             \
    void                                                                                \
    __CLASS__::serialize(Archive& a, unsigned int)                                      \
    {                                                                                   \
        a & BOOST_SERIALIZATION_NVP(**this);                                            \
    }                                                                                   \
    BOOST_SERIALIZATION_PIMPL_EXPLICIT_INSTANTIATIONS(__CLASS__)                        \

#endif // PIMPL_GENERALIZATION_HEADER_VB
