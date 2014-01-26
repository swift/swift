/*
 * Copyright (c) 2010-2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/SecurityLabel.h>

namespace Swift {
	class LuaElementConvertors;

	class SecurityLabelConvertor : public GenericLuaElementConvertor<SecurityLabel> {
		public:
			SecurityLabelConvertor(LuaElementConvertors* convertors);
			virtual ~SecurityLabelConvertor();

			virtual boost::shared_ptr<SecurityLabel> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<SecurityLabel>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
