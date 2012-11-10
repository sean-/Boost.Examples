#ifndef EXAMPLE_HPP
#define EXAMPLE_HPP

#include <memory>
#include <string>

#ifndef BOOST_THREAD_SHARED_MUTEX_HPP
# include <boost/thread/shared_mutex.hpp>
#endif

namespace stackoverflow {

class Example final {
public:
  typedef ::boost::shared_mutex shared_mtx_t;
  typedef ::boost::shared_lock< shared_mtx_t > shared_lock_t;
  typedef ::boost::unique_lock< shared_mtx_t > unique_lock_t;

  Example();
  Example(const std::string& initial_foo);

  ~Example();
  Example(const Example&) = delete;             // Prevent copying
  Example& operator=(const Example&) = delete;  // Prevent assignment

  // Example getter method that supports rvalues
  std::string foo() const;

  // Example setter method using perfect forwarding & move semantics. Anything
  // that's std::string-like will work as a parameter.
  template<typename T>
  bool foo_set(T&& new_val);

  // Begin foo_set() variants required to deal with C types (e.g. char[],
  // char*). The rest of the foo_set() methods here are *NOT* required under
  // normal circumstances.

  // Setup a specialization for const char[] that simply forwards along a
  // std::string. This is preferred over having to explicitly instantiate a
  // bunch of const char[N] templates or possibly std::decay a char[] to a
  // char* (i.e. using a std::string as a container is a Good Thing(tm)).
  //
  // Also, without this, it is required to explicitly instantiate the required
  // variants of const char[N] someplace. For example, in example.cpp:
  //
  // template bool Example::foo_set<const char(&)[6]>(char const (&)[6]);
  // template bool Example::foo_set<const char(&)[7]>(char const (&)[7]);
  // template bool Example::foo_set<const char(&)[8]>(char const (&)[8]);
  // ...
  //
  // Eww. Best to just forward to wrap new_val in a std::string and proxy
  // along the call to foo_set<std::string>().
  template<std::size_t N>
  bool foo_set(const char (&new_val)[N]) { return foo_set(std::string(new_val, N)); }

  // Inline function overloads to support null terminated char* && const
  // char* arguments. If there's a way to reduce this duplication with
  // templates, I'm all ears because I wasn't able to generate a templated
  // versions that didn't conflict with foo_set<T&&>().
  bool foo_set(char* new_val)       { return foo_set(std::string(new_val)); }
  bool foo_set(const char* new_val) { return foo_set(std::string(new_val)); }

  // End of the foo_set() overloads.

  // Example getter method for a POD data type
  bool bar(const std::size_t len, char* dst) const;
  std::size_t bar_capacity() const;

  // Example setter that uses a unique lock to access foo()
  bool bar_set(const std::size_t len, const char* src);

  // Question #1: I can't find any harm in making Impl public because the
  // definition is opaque. Making Impl public, however, greatly helps with
  // implementing Example, which does have access to Example::Impl's
  // interface. This is also preferre, IMO, over using friend.
  class Impl;

private:
  mutable shared_mtx_t rw_mtx_;
  std::unique_ptr<Impl> impl_;
};

} // namespace stackoverflow

#endif // EXAMPLE_HPP
