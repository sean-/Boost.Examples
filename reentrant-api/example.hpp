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

  // Example setter method
  bool foo_set(const std::string& new_val);

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
