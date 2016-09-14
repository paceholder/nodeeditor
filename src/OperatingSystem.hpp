#pragma once

#if                                                                            \
  defined (__CYGWIN__)                                                      || \
  defined (__CYGWIN32__)
#  define NODE_EDITOR_PLATFORM "Cygwin"
#  define NODE_EDITOR_PLATFORM_CYGWIN
#  define NODE_EDITOR_PLATFORM_UNIX
#  define NODE_EDITOR_PLATFORM_WINDOWS
#elif                                                                          \
  defined (_WIN16)                                                          || \
  defined (_WIN32)                                                          || \
  defined (_WIN64)                                                          || \
  defined (__WIN32__)                                                       || \
  defined (__TOS_WIN__)                                                     || \
  defined (__WINDOWS__)
#  define NODE_EDITOR_PLATFORM "Windows"
#  define NODE_EDITOR_PLATFORM_WINDOWS
#elif                                                                          \
  defined (macintosh)                                                       || \
  defined (Macintosh)                                                       || \
  defined (__TOS_MACOS__)                                                   || \
  (defined (__APPLE__) && defined (__MACH__))
#  define NODE_EDITOR_PLATFORM "Mac"
#  define NODE_EDITOR_PLATFORM_MAC
#  define NODE_EDITOR_PLATFORM_UNIX
#elif                                                                          \
  defined (linux)                                                           || \
  defined (__linux)                                                         || \
  defined (__linux__)                                                       || \
  defined (__TOS_LINUX__)
#  define NODE_EDITOR_PLATFORM "Linux"
#  define NODE_EDITOR_PLATFORM_LINUX
#  define NODE_EDITOR_PLATFORM_UNIX
#elif                                                                          \
  defined (__FreeBSD__)                                                     || \
  defined (__OpenBSD__)                                                     || \
  defined (__NetBSD__)                                                      || \
  defined (__bsdi__)                                                        || \
  defined (__DragonFly__)
#  define NODE_EDITOR_PLATFORM "BSD"
#  define NODE_EDITOR_PLATFORM_BSD
#  define NODE_EDITOR_PLATFORM_UNIX
#elif                                                                          \
  defined (sun)                                                             || \
  defined (__sun)
#  define NODE_EDITOR_PLATFORM "Solaris"
#  define NODE_EDITOR_PLATFORM_SOLARIS
#  define NODE_EDITOR_PLATFORM_UNIX
#elif                                                                          \
  defined (_AIX)                                                            || \
  defined (__TOS_AIX__)
#  define NODE_EDITOR_PLATFORM "AIX"
#  define NODE_EDITOR_PLATFORM_AIX
#  define NODE_EDITOR_PLATFORM_UNIX
#elif                                                                          \
  defined (hpux)                                                            || \
  defined (_hpux)                                                           || \
  defined (__hpux)
#  define NODE_EDITOR_PLATFORM "HPUX"
#  define NODE_EDITOR_PLATFORM_HPUX
#  define NODE_EDITOR_PLATFORM_UNIX
#elif \
  defined (__QNX__)
#  define NODE_EDITOR_PLATFORM "QNX"
#  define NODE_EDITOR_PLATFORM_QNX
#  define NODE_EDITOR_PLATFORM_UNIX
#elif                                                                          \
  defined (unix)                                                            || \
  defined (__unix)                                                          || \
  defined (__unix__)
#  define NODE_EDITOR_PLATFORM "Unix"
#  define NODE_EDITOR_PLATFORM_UNIX
#endif

#ifndef NODE_EDITOR_PLATFORM
#  error "Current platform is not supported."
#endif
