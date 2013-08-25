/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/VCard.h>

namespace Swift {
	class VCardConvertor : public GenericLuaElementConvertor<VCard> {
		public:
			VCardConvertor();
			virtual ~VCardConvertor();

			virtual boost::shared_ptr<VCard> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<VCard>) SWIFTEN_OVERRIDE;
	};
}
