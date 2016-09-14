#pragma once

#if                                                               \
  defined (__MINGW32__)                                        || \
  defined (__MINGW64__)
#  define NODE_EDITOR_COMPILER "MinGW"
#  define NODE_EDITOR_COMPILER_MINGW
#elif \
  defined (__GNUC__)
#  define NODE_EDITOR_COMPILER "GNU"
#  define NODE_EDITOR_COMPILER_GNU
#  define NODE_EDITOR_COMPILER_GNU_VERSION_MAJOR __GNUC__
#  define NODE_EDITOR_COMPILER_GNU_VERSION_MINOR __GNUC_MINOR__
#  define NODE_EDITOR_COMPILER_GNU_VERSION_PATCH __GNUC_PATCHLEVEL__
#elif \
  defined (__clang__)
#  define NODE_EDITOR_COMPILER "Clang"
#  define NODE_EDITOR_COMPILER_CLANG
#elif \
  defined (_MSC_VER)
#  define NODE_EDITOR_COMPILER "Microsoft Visual C++"
#  define NODE_EDITOR_COMPILER_MICROSOFT
#elif \
  defined (__BORLANDC__)
#  define NODE_EDITOR_COMPILER "Borland C++ Builder"
#  define NODE_EDITOR_COMPILER_BORLAND
#elif \
  defined (__CODEGEARC__)
#  define NODE_EDITOR_COMPILER "CodeGear C++ Builder"
#  define NODE_EDITOR_COMPILER_CODEGEAR
#elif                                                             \
  defined (__INTEL_COMPILER)                                   || \
  defined (__ICL)
#  define NODE_EDITOR_COMPILER "Intel C++"
#  define NODE_EDITOR_COMPILER_INTEL
#elif                                                             \
  defined (__xlC__)                                            || \
  defined (__IBMCPP__)
#  define NODE_EDITOR_COMPILER "IBM XL C++"
#  define NODE_EDITOR_COMPILER_IBM
#elif \
  defined (__HP_aCC)
#  define NODE_EDITOR_COMPILER "HP aC++"
#  define NODE_EDITOR_COMPILER_HP
#elif \
  defined (__WATCOMC__)
#  define NODE_EDITOR_COMPILER "Watcom C++"
#  define NODE_EDITOR_COMPILER_WATCOM
#endif

#ifndef NODE_EDITOR_COMPILER
#  error "Current compiler is not supported."
#endif
