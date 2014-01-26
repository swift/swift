/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubEventDelete.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubEventDeleteConvertor : public GenericLuaElementConvertor<PubSubEventDelete> {
		public:
			PubSubEventDeleteConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubEventDeleteConvertor();

			virtual boost::shared_ptr<PubSubEventDelete> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubEventDelete>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
