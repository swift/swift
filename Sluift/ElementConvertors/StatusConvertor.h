/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/Status.h>

namespace Swift {
	class StatusConvertor : public GenericLuaElementConvertor<Status> {
		public:
			StatusConvertor();
			virtual ~StatusConvertor();

			virtual boost::shared_ptr<Status> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<Status>) SWIFTEN_OVERRIDE;
	};
}
