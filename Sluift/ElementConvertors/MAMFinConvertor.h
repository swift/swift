/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/MAMFin.h>

namespace Swift {
	class LuaElementConvertors;

	class MAMFinConvertor : public GenericLuaElementConvertor<MAMFin> {
		public:
			MAMFinConvertor(LuaElementConvertors* convertors);
			virtual ~MAMFinConvertor();

			virtual boost::shared_ptr<MAMFin> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<MAMFin>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}

