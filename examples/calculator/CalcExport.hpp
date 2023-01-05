#pragma once

#include <QtNodes/internal/Compiler.hpp>
#include <QtNodes/internal/OperatingSystem.hpp>

#ifdef CALC_PLATFORM_WINDOWS
#  define CALC_EXPORT __declspec(dllexport)
#  define CALC_IMPORT __declspec(dllimport)
#  define CALC_LOCAL
#elif                                                           \
  CALC_COMPILER_GNU_VERSION_MAJOR >= 4                         || \
  defined (CALC_COMPILER_CLANG)
#  define CALC_EXPORT __attribute__((visibility("default")))
#  define CALC_IMPORT __attribute__((visibility("default")))
#  define CALC_LOCAL  __attribute__((visibility("hidden")))
#else
#  define CALC_EXPORT
#  define CALC_IMPORT
#  define CALC_LOCAL
#endif

#ifdef __cplusplus
#  define CALC_DEMANGLED extern "C"
#else
#  define CALC_DEMANGLED
#endif


#if defined (CALC_SHARED) && !defined (CALC_STATIC)
#  ifdef CALC_EXPORTS
#    define CALC_PUBLIC CALC_EXPORT
#  else
#    define CALC_PUBLIC CALC_IMPORT
#  endif
#  define CALC_PRIVATE CALC_LOCAL
#elif !defined (CALC_SHARED) && defined (CALC_STATIC)
#  define CALC_PUBLIC
#  define CALC_PRIVATE
#elif defined (CALC_SHARED) && defined (CALC_STATIC)
#  ifdef CALC_EXPORTS
#    error "Cannot build as shared and static simultaneously."
#  else
#    error "Cannot link against shared and static simultaneously."
#  endif
#else
#  ifdef CALC_EXPORTS
#    error "Choose whether to build as shared or static."
#  else
#    error "Choose whether to link against shared or static."
#  endif
#endif
