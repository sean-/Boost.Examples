#include <sysexits.h>

#include <iostream>
#include <memory>
#include <stdexcept>

#include "example.hpp"

int
main(const int /*argc*/, const char** /*argv*/) {
  using std::cout;
  using std::endl;
  using stackoverflow::Example;

  {
    Example e;
    cout << "Example's foo w/ empty ctor arg: " << e.foo() << endl;
  }

  {
    Example e("foo");
    cout << "Example's foo w/ ctor arg: " << e.foo() << endl;
  }

  try {
    Example e;
    const std::string a_const_str("bbbbbbb");
    e.foo_set(a_const_str);
    std::string a_str("cccccccc");
    e.foo_set(a_str);
    e.foo_set(std::string("ddddd"));
    cout << "Example's bar capacity: " << e.bar_capacity() << endl;
    const std::size_t len = e.bar_capacity();

    // Use unique_ptr to free the memory
    std::unique_ptr<char[]> buf(new char[len +1]);
    if (!e.bar(len, buf.get()))
      throw std::runtime_error("Unable to get bar");
    buf[len] = '\0';
    cout << "Example's bar value: " << buf.get() << endl;
    cout << "Example's foo value: " << e.foo() << endl;

    // Set bar, which has a side effect of calling foo_set
    buf[0] = 'a';
    if (!e.bar_set(len, buf.get()))
      throw std::runtime_error("Unable to set bar");
    cout << "Example's bar value: " << buf.get() << endl;
    cout << "Example's foo value: " << e.foo() << endl;
  } catch (...) {
    return EX_SOFTWARE;
  }

  return EX_OK;
}
