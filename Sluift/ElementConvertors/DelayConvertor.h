/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/Delay.h>

namespace Swift {
	class DelayConvertor : public GenericLuaElementConvertor<Delay> {
		public:
			DelayConvertor();
			virtual ~DelayConvertor();

			virtual boost::shared_ptr<Delay> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<Delay>) SWIFTEN_OVERRIDE;
	};
}
