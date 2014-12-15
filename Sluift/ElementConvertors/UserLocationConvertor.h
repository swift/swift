/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/UserLocation.h>

namespace Swift {
	class LuaElementConvertors;

	class UserLocationConvertor : public GenericLuaElementConvertor<UserLocation> {
		public:
			UserLocationConvertor(LuaElementConvertors* convertors);
			virtual ~UserLocationConvertor();

			virtual boost::shared_ptr<UserLocation> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<UserLocation>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
