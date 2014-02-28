/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Sluift/ElementConvertors/StanzaConvertor.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Presence.h>

namespace Swift {
	class LuaElementConvertors;

	class PresenceConvertor : public StanzaConvertor<Presence> {
		public:
			PresenceConvertor(LuaElementConvertors* convertors);
			virtual ~PresenceConvertor();

			virtual boost::shared_ptr<Presence> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<Presence>) SWIFTEN_OVERRIDE;

			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

			static std::string convertPresenceTypeToString(Presence::Type type);
			static Presence::Type convertPresenceTypeFromString(const std::string& type);

		private:
			LuaElementConvertors* convertors;
	};
}

