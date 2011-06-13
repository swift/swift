/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#if defined(SLUIFT_BUILD_DLL)
#define SLUIFT_API __declspec(dllexport)
#else
#define SLUIFT_API extern
#endif

#include <lua.h>

SLUIFT_API int (luaopen_sluift)(lua_State *L);
