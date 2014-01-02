/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubOwnerAffiliations.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubOwnerAffiliationsConvertor : public GenericLuaElementConvertor<PubSubOwnerAffiliations> {
		public:
			PubSubOwnerAffiliationsConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubOwnerAffiliationsConvertor();

			virtual boost::shared_ptr<PubSubOwnerAffiliations> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubOwnerAffiliations>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
