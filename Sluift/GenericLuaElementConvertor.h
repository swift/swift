/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <lua.hpp>
#include <string>

#include <Swiften/Base/Override.h>
#include <Sluift/LuaElementConvertor.h>
#include <Sluift/Lua/Check.h>
#include <Sluift/Lua/LuaUtils.h>

namespace Swift {
	template<typename T>
	class GenericLuaElementConvertor : public LuaElementConvertor {
		public:
			GenericLuaElementConvertor(const std::string& type) : type(type) {
			}

			virtual ~GenericLuaElementConvertor() {}

			virtual boost::shared_ptr<Element> convertFromLua(lua_State* L, int index, const std::string& payloadType) SWIFTEN_OVERRIDE {
				if (payloadType == type) {
					Lua::checkType(L, index, LUA_TTABLE);
					lua_pushvalue(L, index);
					boost::shared_ptr<Element> result = doConvertFromLua(L);
					lua_pop(L, 1);
					return result;
				}
				return boost::shared_ptr<Element>();
			}

			virtual boost::optional<std::string> convertToLua(
					lua_State* L, boost::shared_ptr<Element> payload) SWIFTEN_OVERRIDE {
				if (boost::shared_ptr<T> actualPayload = boost::dynamic_pointer_cast<T>(payload)) {
					doConvertToLua(L, actualPayload);
					assert(lua_type(L, -1) == LUA_TTABLE);
					return type;
				}
				return NO_RESULT;
			}

		protected:
			virtual boost::shared_ptr<T> doConvertFromLua(lua_State*) = 0;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<T>) = 0;

		private:
			std::string type;
	};
}
