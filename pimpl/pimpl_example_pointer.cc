// Copyright Sean Chittenden 2011. Distributed under the Boost Software
// License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <string>

#include "pimpl_example_pointer.h"

int
main() {
  const char cstr[] = "fo";
  org::example::String str(cstr, sizeof(cstr) - 1);
  str.append('b');

  const std::string& str_ref = str.str_ref();
  const std::string* str_ptr = str.str_ptr();
  { // Modify via mutable reference
    std::string& s = str.str_ref_mutable();
    s.append(2, 'z');
  }

  BOOST_ASSERT(*str.str_ptr() == "fobzz");
  BOOST_ASSERT(str.str_ref() == "fobzz");
  BOOST_ASSERT(str_ref == *str.str_ptr());

  std::cout << " ptr: " << str_ptr << std::endl;
  std::cout << "&ref: " << &str_ref << std::endl;
  std::cout << " ref: " << str_ref << std::endl;
  std::cout << "*ptr: " << *str_ptr << std::endl;

  // Regardless of the size of String's implementation or the number of
  // private data members, it should have a constant size (until you
  // introduce virtual functions).
  std::cout << "sizeof(org::example::String): " << sizeof(str) << std::endl;

  return 0;
}

