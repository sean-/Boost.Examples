#include <iostream>

#include "foo.hpp"

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

  // Mutate newBar to your heart's content, "self" is const protected, go crazy
  auto prep = [](const decltype(*this)& self, decltype(newBar)& dst) {
    dst = self.bar_ + self.bar_;
  };

  // Per http://exceptionsafecode.com/, don't throw below this line -
  // enforced via commit()'s noexcept
  auto commit = [](decltype(newBar)& src, decltype(*this)& dst) noexcept {
    dst.bar_.swap(src);
  };

  prep(*this, newBar);
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
