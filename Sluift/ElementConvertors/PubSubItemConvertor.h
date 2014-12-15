/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubItem.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubItemConvertor : public GenericLuaElementConvertor<PubSubItem> {
		public:
			PubSubItemConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubItemConvertor();

			virtual boost::shared_ptr<PubSubItem> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubItem>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
