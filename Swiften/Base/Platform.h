/*
 * Copyright (c) 2010-2019 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

// Base platforms
#if defined(linux) || defined(__linux) || defined(__linux__)
#define SWIFTEN_PLATFORM_LINUX
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#define SWIFTEN_PLATFORM_BSD
#elif defined(sun) || defined(__sun)
#define SWIFTEN_PLATFORM_SOLARIS
#elif defined(__sgi)
#define SWIFTEN_PLATFORM_SGI
#elif defined(__hpux)
#define SWIFTEN_PLATFORM_HPUX
#elif defined(__CYGWIN__)
#define SWIFTEN_PLATFORM_CYGWIN
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define SWIFTEN_PLATFORM_WIN32
#elif defined(__BEOS__)
#define SWIFTEN_PLATFORM_BEOS
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#define SWIFTEN_PLATFORM_MACOSX
#include <TargetConditionals.h>
#  if TARGET_OS_IPHONE == 1
#  define SWIFTEN_PLATFORM_IPHONE
#  endif
#elif defined(__IBMCPP__) || defined(_AIX)
#define SWIFTEN_PLATFORM_AIX
#elif defined(__amigaos__)
#define SWIFTEN_PLATFORM_AMIGAOS
#elif defined(__QNXNTO__)
#define SWIFTEN_PLATFORM_QNX
#endif

// Derived platforms
#if defined(SWIFTEN_PLATFORM_CYGWIN) || defined(SWIFTEN_PLATFORM_WIN32)
#define SWIFTEN_PLATFORM_WINDOWS
#endif

// Endianness
#include <boost/predef/other/endian.h>
#if defined(BOOST_ENDIAN_LITTLE_BYTE)
#define SWIFTEN_LITTLE_ENDIAN
#elif defined(BOOST_ENDIAN_BIG_BYTE)
#define SWIFTEN_BIG_ENDIAN
#endif
