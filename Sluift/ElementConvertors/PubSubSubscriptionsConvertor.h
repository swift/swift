/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubSubscriptions.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubSubscriptionsConvertor : public GenericLuaElementConvertor<PubSubSubscriptions> {
		public:
			PubSubSubscriptionsConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubSubscriptionsConvertor();

			virtual boost::shared_ptr<PubSubSubscriptions> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubSubscriptions>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
