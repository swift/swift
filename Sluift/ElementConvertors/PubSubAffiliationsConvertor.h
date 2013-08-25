/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubAffiliations.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubAffiliationsConvertor : public GenericLuaElementConvertor<PubSubAffiliations> {
		public:
			PubSubAffiliationsConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubAffiliationsConvertor();

			virtual boost::shared_ptr<PubSubAffiliations> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubAffiliations>) SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
