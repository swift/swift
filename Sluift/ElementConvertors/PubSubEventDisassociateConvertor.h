/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubEventDisassociate.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubEventDisassociateConvertor : public GenericLuaElementConvertor<PubSubEventDisassociate> {
		public:
			PubSubEventDisassociateConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubEventDisassociateConvertor();

			virtual boost::shared_ptr<PubSubEventDisassociate> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubEventDisassociate>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
