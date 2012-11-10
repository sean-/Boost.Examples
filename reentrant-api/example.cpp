#include "example.hpp"

#include <algorithm>
#include <cstring>
#include <utility>

namespace stackoverflow {

class Example;
class Example::Impl;

// Explicitly instantiate std::string variants
template bool Example::foo_set<std::string>(std::string&& src);
template bool Example::foo_set<std::string&>(std::string& src);
template bool Example::foo_set<const std::string&>(const std::string& src);

class Example::Impl final {
public:
  // ctors && obj boilerplate
  Impl();
  Impl(const std::string& init_foo);
  ~Impl() = default;
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;

  // Use a template because we don't care which Lockable concept or LockType
  // is being used, just so long as a lock is held.
  template <typename LockType>
  bool bar(LockType& lk, std::size_t len, char* dst) const;

  template <typename LockType>
  std::size_t bar_capacity(LockType& lk) const;

  // bar_set() requires a unique lock
  bool bar_set(unique_lock_t& lk, const std::size_t len, const char* src);

  template <typename LockType>
  std::string foo(LockType& lk) const;

  template <typename T>
  bool foo_set(unique_lock_t& lk, T&& src);

private:
  // Example datatype that supports rvalue references
  std::string foo_;

  // Example POD datatype that doesn't support rvalue
  static const std::size_t bar_capacity_ = 16;
  char bar_[bar_capacity_ + 1];
};

// Example delegating ctor
Example::Impl::Impl() : Impl("default foo value") {}

Example::Impl::Impl(const std::string& init_foo) : foo_{init_foo} {
  std::memset(bar_, 99 /* ASCII 'c' */, bar_capacity_);
  bar_[bar_capacity_] = '\0'; // null padding
}

template <typename LockType>
bool
Example::Impl::bar(LockType& lk, const std::size_t len, char* dst) const {
  BOOST_ASSERT(lk.owns_lock());
  if (len != bar_capacity(lk))
    return false;
  std::memcpy(dst, bar_, len);

  return true;
}

template <typename LockType>
std::size_t
Example::Impl::bar_capacity(LockType& lk) const {
  BOOST_ASSERT(lk.owns_lock());
  return Impl::bar_capacity_;
}

bool
Example::Impl::bar_set(unique_lock_t &lk, const std::size_t len, const char* src) {
  BOOST_ASSERT(lk.owns_lock());
  // Return false if the size is not the same, or if the values are identical
  if (len != bar_capacity(lk) || foo(lk) == src)
    return false;
  // Copy src to bar_, side effect of updating foo_ if they're different
  std::memcpy(bar_, src, len);
  foo_set(lk, std::string(src, len));
  return true;
}

template <typename LockType>
std::string
Example::Impl::foo(LockType& lk) const {
  BOOST_ASSERT(lk.owns_lock());
  return foo_;
}

template <typename T>
bool
Example::Impl::foo_set(unique_lock_t &lk, T&& src) {
  BOOST_ASSERT(lk.owns_lock());
  if (foo_ == src) return false;
  foo_ = std::move(src);
  return true;
}


Example::Example() : impl_(new Impl{}) {}
Example::Example(const std::string& init_foo) : impl_(new Impl{init_foo}) {}
Example::~Example() = default;

bool
Example::bar(const std::size_t len, char* dst) const {
  shared_lock_t lk(rw_mtx_);
  return impl_->bar(lk, len , dst);
}

std::size_t
Example::bar_capacity() const {
  shared_lock_t lk(rw_mtx_);
  return impl_->bar_capacity(lk);
}

bool
Example::bar_set(const std::size_t len, const char* src) {
  unique_lock_t lk(rw_mtx_);
  return impl_->bar_set(lk, len, src);
}

std::string
Example::foo() const {
  shared_lock_t lk(rw_mtx_);
  return impl_->foo(lk);
}

template<typename T>
bool
Example::foo_set(T&& src) {
  unique_lock_t lk(rw_mtx_);
  return impl_->foo_set(lk, std::forward<T>(src));
}

} // namespace stackoverflow
