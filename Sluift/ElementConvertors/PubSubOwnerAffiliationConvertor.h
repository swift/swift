/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
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
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
