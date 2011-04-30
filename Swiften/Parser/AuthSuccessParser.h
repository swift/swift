/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Parser/GenericElementParser.h>
#include <Swiften/Elements/AuthSuccess.h>
#include <string>

namespace Swift {
	class AuthSuccessParser : public GenericElementParser<AuthSuccess> {
		public:
			AuthSuccessParser();

			virtual void handleStartElement(const std::string&, const std::string& ns, const AttributeMap&);
			virtual void handleEndElement(const std::string&, const std::string& ns);
			virtual void handleCharacterData(const std::string&);

		private:
			int depth;
			std::string text;
	};
}
