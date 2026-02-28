/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/Lua/FunctionRegistration.h>

using namespace Swift::Lua;

FunctionRegistration::FunctionRegistration(const std::string& name, lua_CFunction function, const std::string& type, const std::string& helpDescription, const std::string& helpParameters, const std::string& helpOptions) {
    FunctionRegistry::getInstance().addFunction(name, function, type, helpDescription, helpParameters, helpOptions);
}

FunctionRegistration::~FunctionRegistration() {
}
