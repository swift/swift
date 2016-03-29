/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Base/Override.h>

struct lua_State;

namespace Swift {
	class Element;

	class LuaElementConvertor {
		public:
			static boost::optional<std::string> NO_RESULT;

			struct Documentation {
				Documentation(const std::string& className, const std::string& description) : 
					className(className), description(description) {}
				std::string className;
				std::string description;
			};

			virtual ~LuaElementConvertor();

			virtual boost::shared_ptr<Element> convertFromLua(lua_State*, int index, const std::string& type) = 0;
			virtual boost::optional<std::string> convertToLua(lua_State*, boost::shared_ptr<Element>) = 0;

			virtual boost::optional<Documentation> getDocumentation() const {
				return boost::optional<Documentation>();
			}
	};
}
