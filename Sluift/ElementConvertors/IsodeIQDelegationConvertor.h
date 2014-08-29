/*
 * Copyright (c) 2014 Remko Tronçon and Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/IsodeIQDelegation.h>

namespace Swift {
	class LuaElementConvertors;

	class IsodeIQDelegationConvertor : public GenericLuaElementConvertor<IsodeIQDelegation> {
		public:
			IsodeIQDelegationConvertor(LuaElementConvertors* convertors);
			virtual ~IsodeIQDelegationConvertor();

			virtual boost::shared_ptr<IsodeIQDelegation> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<IsodeIQDelegation>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
