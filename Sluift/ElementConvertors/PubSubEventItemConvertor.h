/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubEventItem.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubEventItemConvertor : public GenericLuaElementConvertor<PubSubEventItem> {
		public:
			PubSubEventItemConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubEventItemConvertor();

			virtual boost::shared_ptr<PubSubEventItem> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubEventItem>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
