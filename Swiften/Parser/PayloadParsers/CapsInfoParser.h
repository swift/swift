/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Elements/CapsInfo.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class CapsInfoParser : public GenericPayloadParser<CapsInfo> {
		public:
			CapsInfoParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			int level;
	};
}
