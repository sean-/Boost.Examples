#include <memory>
#include <mutex>
#include <string>
namespace MyProject {
class Foo {
public:
  typedef std::mutex Mutex;
  typedef std::lock_guard<Mutex> LockGuard;

  Foo(const char* str);
  ~Foo();

  void mutate_bar();
  const std::string& bar() const;
private:
  mutable Mutex mtx_;
  class Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace MyProject
