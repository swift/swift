/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/Body.h>

namespace Swift {
	class LuaElementConvertors;

	class BodyConvertor : public GenericLuaElementConvertor<Body> {
		public:
			BodyConvertor();
			virtual ~BodyConvertor();

			virtual boost::shared_ptr<Body> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<Body>) SWIFTEN_OVERRIDE;
	};
}
