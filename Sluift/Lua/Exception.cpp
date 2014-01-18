/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/Lua/Exception.h>

using namespace Swift::Lua;

Exception::Exception(const std::string& what) : std::runtime_error(what) {
}

Exception::~Exception() SWIFTEN_NOEXCEPT {
}
