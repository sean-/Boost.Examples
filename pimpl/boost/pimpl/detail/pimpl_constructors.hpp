#ifndef PIMPL_CONSTRUCTORS_DETAIL_HEADER_VB
#define PIMPL_CONSTRUCTORS_DETAIL_HEADER_VB

// Copyright 2001-2008 Vladimir Batov.
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. See http://www.boost.org/LICENSE_1_0.txt.

// Series of constructors transferring parameters down to the implementation
// class. That is done to encapsulate object construction inside this class and,
// consequently, *fully* automate memory management (not just deletion).

#if defined(_MSC_VER) && 1500 <= _MSC_VER // Tested with Visual Studio 2008 v9.0.

#define __MANY_MORE_PIMPL_CONSTRUCTORS__                                                \
                                                                                        \
    template<class A>                                                                   \
    pimpl_base(A& a, __DEPLOY_IF_NOT_PIMPL_DERIVED__(A))                                \
    : impl_(new implementation(a)) {}                                                   \
    template<class A, class B>                                                          \
    pimpl_base(A& a, B& b)                                                              \
    : impl_(new implementation(a,b)) {}                                                 \
    template<class A, class B, class C>                                                 \
    pimpl_base(A& a, B& b, C& c)                                                        \
    : impl_(new implementation(a,b,c)) {}                                               \
    template<class A, class B, class C, class D>                                        \
    pimpl_base(A a, B b, C& c, D& d)                                                    \
    : impl_(new implementation(a,b,c,d)) {}                                             \
    template<class A, class B, class C, class D, class E>                               \
    pimpl_base(A& a, B& b, C& c, D& d, E& e)                                            \
    : impl_(new implementation(a,b,c,d,e)) {}                                           \
    template<class A, class B, class C, class D, class E, class F>                      \
    pimpl_base(A& a, B& b, C& c, D& d, E& e, F& f)                                      \
    : impl_(new implementation(a,b,c,d,e,f)) {}                                         \
    template<class A, class B, class C, class D, class E, class F, class G>             \
    pimpl_base(A& a, B& b, C& c, D& d, E& e, F& f, G& g)                                \
    : impl_(new implementation(a,b,c,d,e,f,g)) {}                                       \
    template<class A, class B, class C, class D, class E, class F, class G, class H>    \
    pimpl_base(A& a, B& b, C& c, D& d, E& e, F& f, G& g, H& h)                          \
    : impl_(new implementation(a,b,c,d,e,f,g,h)) {}

#else

// gcc 4.2.3 gets confused by the following:
// Foo create_foo() { ... return foo; }
//
// struct Moo : public pimpl<Moo>::pointer_semantics
// {
//      Moo() : base(create_foo()) {}
// };
// gcc 4.2.3 tries to find a constructor for "base(Foo)" and fails to find the
// matching constructor "template<class A> pimpl_base::pimpl_base(A&)" with
// A = "Foo const". So, I have to resort to the ugly stuff below.
//
// 'const' variations are to preserve const-ness during transfer.
// For example, something like
//      Foo foo;
//      pimpl_base(foo, ...);
// can be handled by template<A1> pimpl_base(A1&).
// However, passing parameteres via a temporary like
//      pimpl_base(Foo(), ...)
// requires template<A1> pimpl_base(A1 const&) and gcc 4.2.3 get confused.

#undef  __PIMPL_CONSTRUCTOR_2_
#undef  __PIMPL_CONSTRUCTOR_3_
#undef  __PIMPL_CONSTRUCTOR_4_
#undef  __PIMPL_CONSTRUCTOR_5_

#define __PIMPL_CONSTRUCTOR_2_(c1, c2)  \
    template<class A1, class A2>        \
    pimpl_base(A1 c1& a1, A2 c2& a2)    \
    : impl_(new implementation(a1,a2)) {}

#define __PIMPL_CONSTRUCTOR_3_(c1, c2, c3)      \
    template<class A1, class A2, class A3>      \
    pimpl_base(A1 c1& a1, A2 c2& a2, A3 c3& a3) \
    : impl_(new implementation(a1,a2,a3)) {}

#define __PIMPL_CONSTRUCTOR_4_(c1, c2, c3, c4)              \
    template<class A1, class A2, class A3, class A4>        \
    pimpl_base(A1 c1& a1, A2 c2& a2, A3 c3& a3, A4 c4& a4)  \
    : impl_(new implementation(a1,a2,a3,a4)) {}

#define __PIMPL_CONSTRUCTOR_5_(c1, c2, c3, c4, c5)                      \
    template<class A1, class A2, class A3, class A4, class A5>          \
    pimpl_base(A1 c1& a1, A2 c2& a2, A3 c3& a3, A4 c4& a4, A5 c5& a5)   \
    : impl_(new implementation(a1,a2,a3,a4,a5)) {}

#define __MANY_MORE_PIMPL_CONSTRUCTORS__                    \
                                                            \
template<class A>                                           \
pimpl_base(A& a, __DEPLOY_IF_NOT_PIMPL_DERIVED__(A))        \
: impl_(new implementation(a)) {}                           \
template<class A>                                           \
pimpl_base(A const& a, __DEPLOY_IF_NOT_PIMPL_DERIVED__(A))  \
: impl_(new implementation(a)) {}                           \
__PIMPL_CONSTRUCTOR_2_ (     ,     )                        \
__PIMPL_CONSTRUCTOR_2_ (     ,const)                        \
__PIMPL_CONSTRUCTOR_2_ (const,     )                        \
__PIMPL_CONSTRUCTOR_2_ (const,const)                        \
__PIMPL_CONSTRUCTOR_3_ (     ,     ,     )                  \
__PIMPL_CONSTRUCTOR_3_ (     ,     ,const)                  \
__PIMPL_CONSTRUCTOR_3_ (     ,const,     )                  \
__PIMPL_CONSTRUCTOR_3_ (     ,const,const)                  \
__PIMPL_CONSTRUCTOR_3_ (const,     ,     )                  \
__PIMPL_CONSTRUCTOR_3_ (const,     ,const)                  \
__PIMPL_CONSTRUCTOR_3_ (const,const,     )                  \
__PIMPL_CONSTRUCTOR_3_ (const,const,const)                  \
__PIMPL_CONSTRUCTOR_4_ (     ,     ,     ,     )            \
__PIMPL_CONSTRUCTOR_4_ (     ,     ,     ,const)            \
__PIMPL_CONSTRUCTOR_4_ (     ,     ,const,     )            \
__PIMPL_CONSTRUCTOR_4_ (     ,     ,const,const)            \
__PIMPL_CONSTRUCTOR_4_ (     ,const,     ,     )            \
__PIMPL_CONSTRUCTOR_4_ (     ,const,     ,const)            \
__PIMPL_CONSTRUCTOR_4_ (     ,const,const,     )            \
__PIMPL_CONSTRUCTOR_4_ (     ,const,const,const)            \
__PIMPL_CONSTRUCTOR_4_ (const,     ,     ,     )            \
__PIMPL_CONSTRUCTOR_4_ (const,     ,     ,const)            \
__PIMPL_CONSTRUCTOR_4_ (const,     ,const,     )            \
__PIMPL_CONSTRUCTOR_4_ (const,     ,const,const)            \
__PIMPL_CONSTRUCTOR_4_ (const,const,     ,     )            \
__PIMPL_CONSTRUCTOR_4_ (const,const,     ,const)            \
__PIMPL_CONSTRUCTOR_4_ (const,const,const,     )            \
__PIMPL_CONSTRUCTOR_4_ (const,const,const,const)            \
__PIMPL_CONSTRUCTOR_5_ (     ,     ,     ,     ,     )      \
__PIMPL_CONSTRUCTOR_5_ (     ,     ,     ,     ,const)      \
__PIMPL_CONSTRUCTOR_5_ (     ,     ,     ,const,     )      \
__PIMPL_CONSTRUCTOR_5_ (     ,     ,     ,const,const)      \
__PIMPL_CONSTRUCTOR_5_ (     ,     ,const,     ,     )      \
__PIMPL_CONSTRUCTOR_5_ (     ,     ,const,     ,const)      \
__PIMPL_CONSTRUCTOR_5_ (     ,     ,const,const,     )      \
__PIMPL_CONSTRUCTOR_5_ (     ,     ,const,const,const)      \
__PIMPL_CONSTRUCTOR_5_ (     ,const,     ,     ,     )      \
__PIMPL_CONSTRUCTOR_5_ (     ,const,     ,     ,const)      \
__PIMPL_CONSTRUCTOR_5_ (     ,const,     ,const,     )      \
__PIMPL_CONSTRUCTOR_5_ (     ,const,     ,const,const)      \
__PIMPL_CONSTRUCTOR_5_ (     ,const,const,     ,     )      \
__PIMPL_CONSTRUCTOR_5_ (     ,const,const,     ,const)      \
__PIMPL_CONSTRUCTOR_5_ (     ,const,const,const,     )      \
__PIMPL_CONSTRUCTOR_5_ (     ,const,const,const,const)      \
__PIMPL_CONSTRUCTOR_5_ (const,     ,     ,     ,     )      \
__PIMPL_CONSTRUCTOR_5_ (const,     ,     ,     ,const)      \
__PIMPL_CONSTRUCTOR_5_ (const,     ,     ,const,     )      \
__PIMPL_CONSTRUCTOR_5_ (const,     ,     ,const,const)      \
__PIMPL_CONSTRUCTOR_5_ (const,     ,const,     ,     )      \
__PIMPL_CONSTRUCTOR_5_ (const,     ,const,     ,const)      \
__PIMPL_CONSTRUCTOR_5_ (const,     ,const,const,     )      \
__PIMPL_CONSTRUCTOR_5_ (const,     ,const,const,const)      \
__PIMPL_CONSTRUCTOR_5_ (const,const,     ,     ,     )      \
__PIMPL_CONSTRUCTOR_5_ (const,const,     ,     ,const)      \
__PIMPL_CONSTRUCTOR_5_ (const,const,     ,const,     )      \
__PIMPL_CONSTRUCTOR_5_ (const,const,     ,const,const)      \
__PIMPL_CONSTRUCTOR_5_ (const,const,const,     ,     )      \
__PIMPL_CONSTRUCTOR_5_ (const,const,const,     ,const)      \
__PIMPL_CONSTRUCTOR_5_ (const,const,const,const,     )      \
__PIMPL_CONSTRUCTOR_5_ (const,const,const,const,const)

// Implement more when needed.
// Hopefully more compilers will be as intelligent as VS C++ 9.0

#endif // _MSC_VER
#endif // PIMPL_CONSTRUCTORS_DETAIL_HEADER_VB
