/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Forwarded.h>

namespace Swift {
	class LuaElementConvertors;

	class ForwardedConvertor : public GenericLuaElementConvertor<Forwarded> {
		public:
			ForwardedConvertor(LuaElementConvertors* convertors);
			virtual ~ForwardedConvertor();

			virtual boost::shared_ptr<Forwarded> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<Forwarded>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
		private:
			LuaElementConvertors* convertors;
	};
}

