/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/VCardUpdate.h>

namespace Swift {
	class VCardUpdateConvertor : public GenericLuaElementConvertor<VCardUpdate> {
		public:
			VCardUpdateConvertor();
			virtual ~VCardUpdateConvertor();

			virtual boost::shared_ptr<VCardUpdate> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<VCardUpdate>) SWIFTEN_OVERRIDE;
	};
}
