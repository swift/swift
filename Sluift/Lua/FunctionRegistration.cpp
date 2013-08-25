/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/Lua/FunctionRegistration.h>

using namespace Swift::Lua;

FunctionRegistration::FunctionRegistration(const std::string& name, lua_CFunction function, const std::string& type) {
	FunctionRegistry::getInstance().addFunction(name, function, type);
}

FunctionRegistration::~FunctionRegistration() {
}
