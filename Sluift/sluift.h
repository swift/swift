/*
 * Copyright (c) 2011-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#if defined(SLUIFT_BUILD_DLL)
#define SLUIFT_API __declspec(dllexport)
#elif __GNUC__ >= 4
#define SLUIFT_API __attribute__((visibility("default")))
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
