/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/Lua/Exception.h>

using namespace Swift::Lua;

Exception::Exception(const std::string& what) : std::runtime_error(what) {
}

Exception::~Exception() SWIFTEN_NOEXCEPT {
}
