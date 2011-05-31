// Copyright Sean Chittenden 2011. Distributed under the Boost Software
// License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "pimpl_example_pointer.h"

// This internal implementation usually only have destructor, constructors,
// data and probably internal methods. Given it is already internal by
// design, it does not need to be a 'class' with "private by default"
// security semantics. All public methods are declared in the external
// visible Test1 class. Then, data in this structure are accessed as
// (*this)->data or (**this).data.
//
// Changes to the contents of this file do not result in consumers to
// recompile (however they will need to relink because the .o file has
// changed).
template<> struct pimpl< org::example::String >::implementation : boost::noncopyable {
  implementation() : str_(0) { }
  implementation(char const* c_str, std::size_t len) : str_(0) {
    str_ = new std::string(c_str, len);
  }
  ~implementation() {
    if (str_)
      delete str_;
  }

  // Consumers of my Interface are not aware of this or any other member
  // variable.
  std::string* str_;

  // Suck up some bytes to demo stable front-end ABI
  int a_;
  int b_;
  char padding[256];
};


// Begin the namespace here and not above (not sure what the source of the
// breakage is, but it breaks).
namespace org {
namespace example {


// Call the implementation ctor/dtors. Forgetting to do this results in
// linker errors.
String::String() : base() { }
String::String(char const* cstr, std::size_t len) : base(cstr, len) { }


bool String::append(unsigned char byte) {
  (*this)->str_->append(1, byte);
  return true;
}


const std::string& String::str_ref() const { return *((*this)->str_); }

std::string& String::str_ref_mutable() const { return *((*this)->str_); }

const std::string* String::str_ptr() const { return (*this)->str_; }

} // namespace example
} // namespace org
