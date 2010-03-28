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
			String currentText_;
	};
}
