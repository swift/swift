/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <lua.hpp>
#include <string>
#include <vector>

namespace Swift {
	namespace Lua {
		class FunctionRegistry {
			public:
				~FunctionRegistry();
				static FunctionRegistry& getInstance();

				void addFunction(const std::string& name, lua_CFunction function, const std::string& type,
						const std::string& helpDescription, const std::string& helpParameters, const std::string& helpOptions);

				void createFunctionTable(lua_State* L, const std::string& type);

				/**
				 * Adds the functions to the table on the top of the stack.
				 */
				void addFunctionsToTable(lua_State* L, const std::string& type);

			private:
				FunctionRegistry();


			private:
				struct Registration {
					std::string name;
					lua_CFunction function;
					std::string type;
					std::string helpDescription;
					std::string helpParameters;
					std::string helpOptions;
				};
				std::vector<Registration> registrations;
		};
	}
}
