/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubUnsubscribe.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubUnsubscribeConvertor : public GenericLuaElementConvertor<PubSubUnsubscribe> {
		public:
			PubSubUnsubscribeConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubUnsubscribeConvertor();

			virtual boost::shared_ptr<PubSubUnsubscribe> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubUnsubscribe>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
