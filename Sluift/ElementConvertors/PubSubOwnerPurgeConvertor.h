/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubOwnerPurge.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubOwnerPurgeConvertor : public GenericLuaElementConvertor<PubSubOwnerPurge> {
		public:
			PubSubOwnerPurgeConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubOwnerPurgeConvertor();

			virtual boost::shared_ptr<PubSubOwnerPurge> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubOwnerPurge>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
