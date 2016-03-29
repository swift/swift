/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/SoftwareVersion.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
	class SoftwareVersionConvertor : public GenericLuaElementConvertor<SoftwareVersion> {
		public:
			SoftwareVersionConvertor();
			virtual ~SoftwareVersionConvertor();

			virtual boost::shared_ptr<SoftwareVersion> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<SoftwareVersion>) SWIFTEN_OVERRIDE;
	};
}
