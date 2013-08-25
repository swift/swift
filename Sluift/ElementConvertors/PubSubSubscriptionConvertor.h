/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubSubscription.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubSubscriptionConvertor : public GenericLuaElementConvertor<PubSubSubscription> {
		public:
			PubSubSubscriptionConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubSubscriptionConvertor();

			virtual boost::shared_ptr<PubSubSubscription> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubSubscription>) SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
