/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubSubscribe.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubSubscribeConvertor : public GenericLuaElementConvertor<PubSubSubscribe> {
		public:
			PubSubSubscribeConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubSubscribeConvertor();

			virtual boost::shared_ptr<PubSubSubscribe> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubSubscribe>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
