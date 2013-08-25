/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
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

		private:
			LuaElementConvertors* convertors;
	};
}
