/*
 * Copyright (c) 2014 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/StatusShow.h>

namespace Swift {
	class StatusShowConvertor : public GenericLuaElementConvertor<StatusShow> {
		public:
			StatusShowConvertor();
			virtual ~StatusShowConvertor();

			virtual boost::shared_ptr<StatusShow> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<StatusShow>) SWIFTEN_OVERRIDE;

			static std::string convertStatusShowTypeToString(const StatusShow::Type &show);
			static StatusShow::Type convertStatusShowTypeFromString(const std::string& show);
	};
}
