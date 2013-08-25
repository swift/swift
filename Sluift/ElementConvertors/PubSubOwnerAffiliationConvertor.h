/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubOwnerAffiliation.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubOwnerAffiliationConvertor : public GenericLuaElementConvertor<PubSubOwnerAffiliation> {
		public:
			PubSubOwnerAffiliationConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubOwnerAffiliationConvertor();

			virtual boost::shared_ptr<PubSubOwnerAffiliation> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubOwnerAffiliation>) SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
