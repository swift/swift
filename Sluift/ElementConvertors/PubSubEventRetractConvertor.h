/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubEventRetract.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubEventRetractConvertor : public GenericLuaElementConvertor<PubSubEventRetract> {
		public:
			PubSubEventRetractConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubEventRetractConvertor();

			virtual boost::shared_ptr<PubSubEventRetract> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubEventRetract>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
