/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubEventPurge.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubEventPurgeConvertor : public GenericLuaElementConvertor<PubSubEventPurge> {
		public:
			PubSubEventPurgeConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubEventPurgeConvertor();

			virtual boost::shared_ptr<PubSubEventPurge> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubEventPurge>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
