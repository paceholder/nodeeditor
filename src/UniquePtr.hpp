#pragma once

#include <memory>
#include "Compiler.hpp"

#if !defined(_MSC_VER) ||                     \
  (defined(_MSC_VER) && (_MSC_VER < 1900))

namespace std {

template<typename T, typename ... Args>
std::unique_ptr<T>
make_unique( Args && ... args )
{
  return std::unique_ptr<T>( new T( std::forward<Args>(args) ... ) );
}
}

#endif
