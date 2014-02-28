/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/MAMResult.h>

namespace Swift {
	class LuaElementConvertors;

	class MAMResultConvertor : public GenericLuaElementConvertor<MAMResult> {
		public:
			MAMResultConvertor(LuaElementConvertors* convertors);
			virtual ~MAMResultConvertor();

			virtual boost::shared_ptr<MAMResult> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<MAMResult>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}

