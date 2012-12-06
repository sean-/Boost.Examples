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

void Foo::Impl::mutate_bar() {
  std::string newBar;

  auto prep = [](const decltype(this)& src, decltype(newBar)& dst) {
    dst = src->bar_ + src->bar_;
  };
  auto commit = [](decltype(newBar)& src, decltype(*this)& dst) {
    dst.bar_.swap(src);
  };

  prep(this, newBar);
  commit(newBar, *this);
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
