/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubEventItems.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubEventItemsConvertor : public GenericLuaElementConvertor<PubSubEventItems> {
		public:
			PubSubEventItemsConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubEventItemsConvertor();

			virtual boost::shared_ptr<PubSubEventItems> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubEventItems>) SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
