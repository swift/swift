/*
 * Copyright (c) 2012-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/Platform.h>

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
