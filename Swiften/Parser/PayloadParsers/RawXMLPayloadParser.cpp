#include "Swiften/Parser/PayloadParsers/RawXMLPayloadParser.h"
#include "Swiften/Parser/SerializingParser.h"

namespace Swift {

RawXMLPayloadParser::RawXMLPayloadParser() : level_(0) {
}

void RawXMLPayloadParser::handleStartElement(const String& element, const String& ns, const AttributeMap& attributes) {
	++level_;
	serializingParser_.handleStartElement(element, ns, attributes);
}

void RawXMLPayloadParser::handleEndElement(const String& element, const String& ns) {
	serializingParser_.handleEndElement(element, ns);
	--level_;
	if (level_ == 0) {
		getPayloadInternal()->setRawXML(serializingParser_.getResult());
	}
}

void RawXMLPayloadParser::handleCharacterData(const String& data) {
	serializingParser_.handleCharacterData(data);
}

}
