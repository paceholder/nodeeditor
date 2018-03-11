#pragma once

#include <memory>


#if (!defined(_MSC_VER) && (__cplusplus < 201300)) || \
    ( defined(_MSC_VER) && (_MSC_VER < 1800))
//_MSC_VER == 1800 is Visual Studio 2013, which is already somewhat C++14 compilant, 
// and it has make_unique in it's standard library implementation

namespace std
{
#if _MSC_VER > 1600
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#else

template<typename T>
std::unique_ptr<T> make_unique()
{
    return std::unique_ptr<T>(new T());
}

template<typename T, typename Arg>
std::unique_ptr<T> make_unique(Arg&& arg)
{
    return std::unique_ptr<T>(new T(std::forward<Arg>(arg)));
}

template<typename T, typename Arg1, typename Arg2>
std::unique_ptr<T> make_unique(Arg1&& arg1, Arg2&& arg2)
{
    return std::unique_ptr<T>(new T(arg1, arg2));
}

#endif
}

#endif
