/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_StartSessionParser_H
#define SWIFTEN_StartSessionParser_H

#include "Swiften/Elements/StartSession.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class StartSessionParser : public GenericPayloadParser<StartSession> {
		public:
			StartSessionParser() {}

			virtual void handleStartElement(const std::string&, const std::string&, const AttributeMap&) {}
			virtual void handleEndElement(const std::string&, const std::string&) {}
			virtual void handleCharacterData(const std::string&) {}
	};
}

#endif
