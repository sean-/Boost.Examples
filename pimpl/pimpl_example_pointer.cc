#include <string>

#include "pimpl_example_pointer.h"

int
main() {
  const char cstr[] = "fo";
  my::String str(cstr, sizeof(cstr) - 1);
  str.append('b');

  BOOST_ASSERT(*str.cur_str() == "fob");
  return 0;
}

