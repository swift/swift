#pragma once

#include "Swiften/Elements/RawXMLPayload.h"
#include "Swiften/Parser/GenericPayloadParser.h"
#include "Swiften/Parser/SerializingParser.h"

namespace Swift {
	class SerializingParser;

	class RawXMLPayloadParser : public GenericPayloadParser<RawXMLPayload> {
		public:
			RawXMLPayloadParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			int level_;
			SerializingParser serializingParser_;
	};
}
