#pragma once

#include <memory>
#include <utility>

namespace QtNodes
{
  namespace detail {
#if (!defined(_MSC_VER) && (__cplusplus < 201300)) || \
    ( defined(_MSC_VER) && (_MSC_VER < 1800)) 
//_MSC_VER == 1800 is Visual Studio 2013, which is already somewhat C++14 compilant, 
// and it has make_unique in it's standard library implementation
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
      return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
#else
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
      return std::make_unique<T>(std::forward<Args>(args)...);
    }
#endif
  }
}
