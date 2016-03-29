/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubAffiliations.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubAffiliationsConvertor : public GenericLuaElementConvertor<PubSubAffiliations> {
		public:
			PubSubAffiliationsConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubAffiliationsConvertor();

			virtual boost::shared_ptr<PubSubAffiliations> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubAffiliations>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
