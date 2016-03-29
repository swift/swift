/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubOwnerDelete.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubOwnerDeleteConvertor : public GenericLuaElementConvertor<PubSubOwnerDelete> {
		public:
			PubSubOwnerDeleteConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubOwnerDeleteConvertor();

			virtual boost::shared_ptr<PubSubOwnerDelete> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubOwnerDelete>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
