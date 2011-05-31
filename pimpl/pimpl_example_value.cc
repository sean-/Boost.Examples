// Copyright Sean Chittenden 2011. Distributed under the Boost Software
// License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <string>

#include "pimpl_example_value.h"

int
main() {
  const char cstr[] = "fo";
  org::example::String str(cstr, sizeof(cstr) - 1);
  str.append('b');

  BOOST_ASSERT(*str.cur_str() == "fob");
  std::cout << "sizeof(org::example::String): " << sizeof(str) << std::endl;
  return 0;
}

