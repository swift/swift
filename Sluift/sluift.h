/*
 * Copyright (c) 2011-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#if defined(SLUIFT_BUILD_DLL)
#define SLUIFT_API __declspec(dllexport)
#else
#define SLUIFT_API
#endif

#if defined(__cplusplus)
#include <lua.hpp>
#else
#include <lua.h>
#endif

#if defined(__cplusplus)
extern "C"
#endif
SLUIFT_API int (luaopen_sluift)(lua_State *L);
