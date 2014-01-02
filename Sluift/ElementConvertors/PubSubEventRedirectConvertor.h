/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubEventRedirect.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubEventRedirectConvertor : public GenericLuaElementConvertor<PubSubEventRedirect> {
		public:
			PubSubEventRedirectConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubEventRedirectConvertor();

			virtual boost::shared_ptr<PubSubEventRedirect> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubEventRedirect>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
