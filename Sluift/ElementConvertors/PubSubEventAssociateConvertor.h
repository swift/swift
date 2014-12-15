/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubEventAssociate.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubEventAssociateConvertor : public GenericLuaElementConvertor<PubSubEventAssociate> {
		public:
			PubSubEventAssociateConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubEventAssociateConvertor();

			virtual boost::shared_ptr<PubSubEventAssociate> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubEventAssociate>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
