/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/LuaElementConvertor.h>

namespace Swift {
	class DefaultElementConvertor : public LuaElementConvertor {
		public:
			DefaultElementConvertor();
			virtual ~DefaultElementConvertor();

			virtual boost::shared_ptr<Element> convertFromLua(lua_State*, int index, const std::string& type) SWIFTEN_OVERRIDE;
			virtual boost::optional<std::string> convertToLua(lua_State*, boost::shared_ptr<Element>) SWIFTEN_OVERRIDE;
	};
}
