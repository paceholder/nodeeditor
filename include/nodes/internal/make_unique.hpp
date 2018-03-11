#pragma once

#include <memory>


#if (!defined(_MSC_VER) && (__cplusplus < 201300)) || \
    ( defined(_MSC_VER) && (_MSC_VER < 1800)) 
//_MSC_VER == 1800 is Visual Studio 2013, which is already somewhat C++14 compilant, 
// and it has make_unique in it's standard library implementation

namespace std
{
  template<typename T, typename... Args>
  std::unique_ptr<T> make_unique(Args&&... args)
  {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
  }
}

#endif
