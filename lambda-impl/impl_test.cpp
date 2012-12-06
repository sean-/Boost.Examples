#include <iostream>

#include "impl_test.hpp"

namespace MyProject {
class Foo;
class Foo::Impl;

class Foo::Impl {
public:
  Impl(const char* str) : bar_(str) {}
  const std::string& bar() const { return bar_; }
  void mutate_bar();

  std::string bar_;
};

namespace impl_helper {

void bar_1stage(const std::string& src, std::string& dst) {
  dst = src + src;
}

void bar_2commit(std::string& src, std::string& dst) {
  dst.swap(src);
}

} // namespace impl_helper

void Foo::Impl::mutate_bar() {
  std::string newBar;
  impl_helper::bar_1stage(bar_, newBar);
  impl_helper::bar_2commit(newBar, bar_);
};

Foo::Foo(const char* str) : impl_(new Foo::Impl(str)) {}
Foo::~Foo() {}

void Foo::mutate_bar() {
  LockGuard lk(mtx_);
  impl_->mutate_bar();
}

const std::string& Foo::bar() const {
  LockGuard lk(mtx_);
  return impl_->bar();
}

} // MyProject

int main() {
  MyProject::Foo f("asdf");
  std::cout << "f.bar(): " << f.bar() << std::endl;
  f.mutate_bar();
  std::cout << "f.bar(): " << f.bar() << std::endl;
  return 0;
}
