/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/MAMArchived.h>

namespace Swift {
	class LuaElementConvertors;

	class MAMArchivedConvertor : public GenericLuaElementConvertor<MAMArchived> {
		public:
			MAMArchivedConvertor(LuaElementConvertors* convertors);
			virtual ~MAMArchivedConvertor();

			virtual boost::shared_ptr<MAMArchived> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<MAMArchived>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}

