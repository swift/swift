/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/Lua/FunctionRegistry.h>

#include <Swiften/Base/foreach.h>

using namespace Swift::Lua;

FunctionRegistry::FunctionRegistry() {
}

FunctionRegistry::~FunctionRegistry() {
}

FunctionRegistry& FunctionRegistry::getInstance() {
	static FunctionRegistry instance;
	return instance;
}

void FunctionRegistry::addFunction(const std::string& name, lua_CFunction function, const std::string& type) {
	Registration registration;
	registration.name = name;
	registration.function = function;
	registration.type = type;
	registrations.push_back(registration);
}

std::string FunctionRegistry::getMetaTableNameForType(const std::string& type) {
	return "Sluift_" + type;
}

void FunctionRegistry::registerTypeMetaTable(lua_State* L, const std::string& type) {
	luaL_newmetatable(L, getMetaTableNameForType(type).c_str());
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	addFunctionsToTable(L, type);
}

void FunctionRegistry::createFunctionTable(lua_State* L, const std::string& type) {
	lua_newtable(L);
	addFunctionsToTable(L, type);
}

void FunctionRegistry::addFunctionsToTable(lua_State* L, const std::string& type) {
	foreach(const Registration& registration, registrations) {
		if (registration.type == type) {
			lua_pushcclosure(L, registration.function, 0);
			lua_setfield(L, -2, registration.name.c_str());
		}
	}
}
