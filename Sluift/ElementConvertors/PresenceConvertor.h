/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Presence.h>

#include <Sluift/ElementConvertors/StanzaConvertor.h>

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

