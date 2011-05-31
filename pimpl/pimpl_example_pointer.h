// Copyright Sean Chittenden 2011. Distributed under the Boost Software
// License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef PIMPL_EXAMPLE_POINTER_H
#define PIMPL_EXAMPLE_POINTER_H

#include "boost/pimpl/pimpl.hpp"

namespace org {
namespace example {

class String : public pimpl< String >::pointer_semantics {
 public:
  // A pure org::String interface.

  // The implementation is hidden in pimpl_example_pointer.cpp. Consumers of
  // this interface need only include this header file.

  /// ctor
  String();

  /// ctor with initial pointer
  String(char const* c_str, std::size_t len);

  /// Add a byte
  bool append(unsigned char byte);

  /// Gets the current string as a reference
  const std::string& str_ref() const;

  /// Gets a mutable reference to the string
  std::string& str_ref_mutable() const;

  /// Gets the current string as a pointer
  const std::string* str_ptr() const;
 private:
  // Possible to reserve additional bytes to maintain ABI stability
  //char padding[32];
};

} // namespace example
} // namespace org

#endif // PIMPL_EXAMPLE_POINTER_H
