/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubSubscribeOptions.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubSubscribeOptionsConvertor : public GenericLuaElementConvertor<PubSubSubscribeOptions> {
		public:
			PubSubSubscribeOptionsConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubSubscribeOptionsConvertor();

			virtual boost::shared_ptr<PubSubSubscribeOptions> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubSubscribeOptions>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
