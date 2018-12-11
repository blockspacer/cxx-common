////////////////////////////////////////////////////////////////////////////////
/// Copyright 2018 Steven C. Wilson
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to
/// deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
/// IN THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstddef>
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////
// The purpose of this header is to auto-define OS/CPU/Compiler based macros
// that use clear and consistent names.  Auto-defining helps ease the need for
// build defines. (SCW: Note that some of these *have* been moved to the
// CMakefile build to remove the need for this to be the first include in all cases)
//
// As an example, instead of needing to know the disparate "WIN32" and
// "__APPLE__" macros, you would instead just need to know "SCW_OS" and that it
// equals "WINDOWS" or "APPLE".  You could also use "SCW_WINDOWS" which is only
// defined on Windows, or "SCW_APPLE" only defined on Apple.
//
// It's imperative that this header relies on nothing outside of standard C++11
// libs to enable easy transport of code that uses this
////////////////////////////////////////////////////////////////////////////////

// OS Related
#if !defined(SCW_OS)
#  if defined(WIN32)
#    define SCW_WINDOWS 1
#    define SCW_OS WINDOWS
#    define NOMINMAX
// Windows version?
#  elif defined(__APPLE__)
#    define SCW_MACOS 1
#    define SCW_UNIX 1
#    define SCW_POSIX 1
#    define SCW_OS MACOS
// MacOS version?
#  elif defined(__linux__)
#    define SCW_UNIX 1
#    define SCW_POSIX 1
#    define SCW_LINUX 1
#    define SCW_OS LINUX
#  else
#    error "Implement This OS"
#  endif
#endif

// Compiler Related - Add anything needed for later
#if !defined(SCW_CXX)
#  if defined(_MSC_VER)
// MS C++
#    define SCW_MSVC_CXX 1
#    define SCW_CXX MSVC
#  elif defined(__clang__)
// CLANG
#    define SCW_CLANG_CXX 1
#    define SCW_CXX CLANG
#  elif defined(__GNUC__)
// GCC
#    define SCW_GCC_CXX 1
#    define SCW_CXX GCC
#  else
#    error "Implement This Compiler"
#  endif
#endif

// Setup well-known 32/64 bit defs.
// SCW_CPU_32BIT or SCW_CPU_64BIT
// SCW: well... I went to try out some 32-bit code on my mac and it complained that 32-bit
//  is deprecated. I guess that's where we're at. I'll take! Maybe I can just remove these
#if !defined(SCW_SIZEOF_POINTER)
#  if !defined(UINTPTR_MAX)
#    error Need UINTPTR_MAX in <cstddef>
#  endif
#  if UINTPTR_MAX == UINT32_MAX
#    error 32-bit not supported
#    define SCW_ARCH_32BIT 1
#    define SCW_CPU_32BIT 1
#    define SCW_SIZEOF_POINTER 4
#    define SCW_ALIGNED_SIZE 4
#    define SCW_MAX_POD_ALIGN_SIZE 8
#  elif UINTPTR_MAX == UINT64_MAX
#    define SCW_ARCH_64BIT 1
#    define SCW_CPU_64BIT 1
#    define SCW_SIZEOF_POINTER 8
#    define SCW_ALIGNED_SIZE 8
#    define SCW_MAX_POD_ALIGN_SIZE 8
#  else
#    error Unexpected CPU Architecture
#  endif
#endif

// Byte order
// SCW: well... Is there any bigendian even left out there? Maybe PPC for safety critical.
//  Similar to the 32-bit... maybe it's about time to just get rid of this
#if !defined(SCW_ENDIAN)
#  if defined(_LIBCPP_LITTLE_ENDIAN) || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || SCW_WINDOWS
// Note - The Windows check is speculative - assuming windows doesn't run big
// endian anywhere
#    define SCW_LITTLE_ENDIAN 1
#    define SCW_ENDIAN LITTLE
#  elif defined(_LIBCPP_BIG_ENDIAN) || (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#    define SCW_BIG_ENDIAN 1
#    define SCW_ENDIAN BIG
#  endif
#endif

#if !defined(SCW_ENDIAN)
#  error "Figure out endian-ness defines for this platform"
#endif

// This will set SCW_CXX_VERSION
#if !defined(SCW_CXX_VERSION)
#  ifdef _MSC_VER
// The MSVC __cplusplus value is not typically correct, although it might start
// being correct as of April 2018.
// https://blogs.msdn.microsoft.com/vcblog/2018/04/09/msvc-now-correctly-reports-__cplusplus/
// For now, we just base our version on the _MSC_VER and/or _MSVC_LANG.
// _MSVC_LANG has only been in existance since VS2015u3. It mirrors what
// __cplusplus should be. Thanks MSFT :)
#    if defined(_MSVC_LANG)
#      if _MSVC_LANG == 199711L
#        error Unsupported VisualStudio version
#      elif _MSVC_LANG == 201103L
#        define SCW_CXX_VERSION 2011L
#      elif _MSVC_LANG == 201402L
#        define SCW_CXX_VERSION 2014L
#      elif _MSVC_LANG == 201703L
#        define SCW_CXX_VERSION 2017L
#      else
#        error Unknown Visual-C++ version
#      endif
#    else
#      if _MSC_VER >= 1500
#        define SCW_CXX_VERSION 2011L
#      else
#        error Unsupported Visual-C++ Version
#      endif
#    endif
#  else
// GCC/Clang
#    if __cplusplus == 199711L
#      define SCW_CXX_VERSION 1998L
#      define SCW_CXX_98 1
#    elif __cplusplus == 201103L
#      define SCW_CXX_VERSION 2011L
#      define SCW_CXX_11 1
#    elif __cplusplus == 201402L
#      define SCW_CXX_VERSION 2014L
#      define SCW_CXX_14 1
#    elif __cplusplus == 201703L
#      define SCW_CXX_VERSION 2017L
#      define SCW_CXX_17 1
#    else
#      error Unknown C++ version: __cplusplus
#    endif
#  endif
#endif
