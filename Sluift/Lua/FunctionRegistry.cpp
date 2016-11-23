/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/Lua/FunctionRegistry.h>

#include <Sluift/Lua/Exception.h>
#include <Sluift/Lua/LuaUtils.h>
#include <Sluift/globals.h>

using namespace Swift::Lua;

FunctionRegistry::FunctionRegistry() {
}

FunctionRegistry::~FunctionRegistry() {
}

FunctionRegistry& FunctionRegistry::getInstance() {
    static FunctionRegistry instance;
    return instance;
}

void FunctionRegistry::addFunction(
        const std::string& name, lua_CFunction function, const std::string& type,
        const std::string& helpDescription, const std::string& helpParameters, const std::string& helpOptions) {
    Registration registration;
    registration.name = name;
    registration.function = function;
    registration.type = type;
    registration.helpDescription = helpDescription;
    registration.helpParameters = helpParameters;
    registration.helpOptions = helpOptions;
    registrations.push_back(registration);
}

void FunctionRegistry::createFunctionTable(lua_State* L, const std::string& type) {
    lua_newtable(L);
    addFunctionsToTable(L, type);
}

void FunctionRegistry::addFunctionsToTable(lua_State* L, const std::string& type) {
    for (const auto& registration : registrations) {
        if (registration.type == type) {
            lua_pushcclosure(L, registration.function, 0);
            if (!registration.helpDescription.empty()) {
                Lua::registerHelp(L, -1, registration.helpDescription, registration.helpParameters, registration.helpOptions);
            }
            else {
                Lua::registerExtraHelp(L, -1, registration.type + "." + registration.name);
            }
            lua_setfield(L, -2, registration.name.c_str());
        }
    }
}
