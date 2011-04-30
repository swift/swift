/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/VCard.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
	class SerializingParser;

	class VCardParser : public GenericPayloadParser<VCard> {
		public:
			VCardParser();

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
			virtual void handleEndElement(const std::string& element, const std::string&);
			virtual void handleCharacterData(const std::string& data);

		private:
			std::string getElementHierarchy() const;

		private:
			std::vector<std::string> elementStack_;
			VCard::EMailAddress currentEMailAddress_;
			SerializingParser* unknownContentParser_;
			std::string currentText_;
	};
}
