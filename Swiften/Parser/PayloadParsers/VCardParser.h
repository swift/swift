/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Elements/VCard.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class SerializingParser;

	class VCardParser : public GenericPayloadParser<VCard> {
		public:
			VCardParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			String getElementHierarchy() const;

		private:
			std::vector<String> elementStack_;
			VCard::EMailAddress currentEMailAddress_;
			SerializingParser* unknownContentParser_;
			String currentText_;
	};
}
