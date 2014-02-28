/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Sluift/ElementConvertors/StanzaConvertor.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/IQ.h>

namespace Swift {
	class LuaElementConvertors;

	class IQConvertor : public StanzaConvertor<IQ> {
		public:
			IQConvertor(LuaElementConvertors* convertors);
			virtual ~IQConvertor();

			virtual boost::shared_ptr<IQ> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<IQ>) SWIFTEN_OVERRIDE;

			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

			static std::string convertIQTypeToString(IQ::Type type);
			static IQ::Type convertIQTypeFromString(const std::string& type);

		private:
			LuaElementConvertors* convertors;
	};
}

