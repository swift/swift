/*
 * Copyright (c) 2012-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Platform.h>
#include <boost/config.hpp>

#ifdef SWIFTEN_STATIC
# define SWIFTEN_API
#else
# ifdef SWIFTEN_PLATFORM_WINDOWS
#  ifdef SWIFTEN_BUILDING
#   define SWIFTEN_API __declspec(dllexport)
#  else
#   define SWIFTEN_API __declspec(dllimport)
#  endif
# elif __GNUC__ >= 4
#  define SWIFTEN_API __attribute__((visibility("default")))
# else
#  define SWIFTEN_API
# endif
#endif

#ifdef BOOST_NO_DEFAULTED_FUNCTIONS
#  define SWIFTEN_DEFAULT_COPY_CONSTRUCTOR(cls)
#  define SWIFTEN_DEFAULT_COPY_ASSIGMNENT_OPERATOR(cls)
#else
#  define SWIFTEN_DEFAULT_COPY_CONSTRUCTOR(cls) \
        cls(const cls&) = default;
#  define SWIFTEN_DEFAULT_COPY_ASSIGMNENT_OPERATOR(cls) \
        cls& operator=(const cls&) = default;
#endif

#ifdef BOOST_NO_NOEXCEPT
#define SWIFTEN_NOEXCEPT throw()
#else
#define SWIFTEN_NOEXCEPT noexcept
#endif
