/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
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

				void addFunction(const std::string& name, lua_CFunction function, const std::string& type);

				static std::string getMetaTableNameForType(const std::string& type);
				void registerTypeMetaTable(lua_State* L, const std::string& type);

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
				};
				std::vector<Registration> registrations;
		};
	}
}
