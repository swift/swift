/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/Command.h>

namespace Swift {
	class LuaElementConvertors;

	class CommandConvertor : public GenericLuaElementConvertor<Command> {
		public:
			CommandConvertor(LuaElementConvertors* convertors);
			virtual ~CommandConvertor();

			virtual boost::shared_ptr<Command> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<Command>) SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
