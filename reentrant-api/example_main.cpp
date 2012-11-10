#include <sysexits.h>

#include <cassert>
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
    { // Test assignment from std::string
      std::string str("cccccccc");
      e.foo_set(str);
      assert(e.foo() == "cccccccc");  // Value is the same
      assert(str.empty());            // Stole the contents of a_str
    }
    { // Test assignment from a const std::string
      const std::string const_str("bbbbbbb");
      e.foo_set(const_str);
      assert(const_str == "bbbbbbb");               // Value is the same
      assert(const_str.c_str() != e.foo().c_str()); // Made a copy
    }
    {
      // Test a const char[7] and a temporary std::string
      e.foo_set("foobar");
      e.foo_set(std::string("ddddd"));
    }
    { // Test char[7]
      char buf[7] = {"foobar"};
      e.foo_set(buf);
      assert(e.foo() == "foobar");
    }
    { //// And a *char[] & const *char[]
      auto buf = new char[7];
      std::memcpy(buf, "foobar", 6);
      buf[6] = '\0';
      e.foo_set(buf);
      const char* const_ptr = buf;
      e.foo_set(const_ptr);
      delete[] buf;
      assert(e.foo() == "foobar");
    }

    cout << "Example's bar capacity: " << e.bar_capacity() << endl;
    const std::size_t len = e.bar_capacity();

    // Use unique_ptr to automatically free buf
    std::unique_ptr<char[]> buf(new char[len +1]);

    // Copy bar in to buf
    if (!e.bar(len, buf.get()))
      throw std::runtime_error("Unable to get bar");
    buf[len] = '\0'; // Null terminate the C string
    cout << endl << "foo and bar (a.k.a.) have different values:" << endl;
    cout << "Example's foo value: " << e.foo() << endl;
    cout << "Example's bar value: " << buf.get() << endl;

    // Set bar, which has a side effect of calling foo_set()
    buf[0] = 'c'; buf[1] = buf[2] = '+'; buf[3] = '\0';
    if (!e.bar_set(sizeof("c++") - 1, buf.get()))
      throw std::runtime_error("Unable to set bar");

    cout << endl << "foo and bar now have identical values but only one lock was acquired when setting:" << endl;
    cout << "Example's foo value: " << e.foo() << endl;
    cout << "Example's bar value: " << buf.get() << endl;
  } catch (...) {
    return EX_SOFTWARE;
  }

  return EX_OK;
}
