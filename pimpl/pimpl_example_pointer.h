#ifndef PIMPL_EXAMPLE_POINTER_H
#define PIMPL_EXAMPLE_POINTER_H

#include "boost/pimpl/pimpl.hpp"

// My interface
namespace my {

class String : public pimpl< String >::pointer_semantics {
 public:
  // A pure my::String interface.

  // The implementation is hidden in pimpl_example_pointer.cpp. Consumers of
  // this interface need only include this header file.

  /// ctor
  String();

  /// ctor with initial value
  String(char const* c_str, std::size_t len);

  /// Add a byte
  bool append(unsigned char byte);

  /// Gets the current value
  const std::string* cur_str() const;
};

} // namespace my

#endif // PIMPL_EXAMPLE_POINTER_H
