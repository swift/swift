#include "Swiften/Parser/PayloadParsers/PrivateStorageParser.h"
#include "Swiften/Parser/PayloadParserFactoryCollection.h"
#include "Swiften/Parser/PayloadParserFactory.h"

namespace Swift {

PrivateStorageParser::PrivateStorageParser(PayloadParserFactoryCollection* factories) : factories(factories), level(0) {
}

void PrivateStorageParser::handleStartElement(const String& element, const String& ns, const AttributeMap& attributes) {
	if (level == 1) {
		PayloadParserFactory* payloadParserFactory = factories->getPayloadParserFactory(element, ns, attributes);
		if (payloadParserFactory) {
			currentPayloadParser.reset(payloadParserFactory->createPayloadParser());
		}
	}

	if (level >= 1 && currentPayloadParser.get()) {
		currentPayloadParser->handleStartElement(element, ns, attributes);
	}
	++level;
}

void PrivateStorageParser::handleEndElement(const String& element, const String& ns) {
	--level;
	if (currentPayloadParser.get()) {
		if (level >= 1) {
			currentPayloadParser->handleEndElement(element, ns);
		}

		if (level == 1) {
			getPayloadInternal()->setPayload(currentPayloadParser->getPayload());
		}
	}
}

void PrivateStorageParser::handleCharacterData(const String& data) {
	if (level > 1 && currentPayloadParser.get()) {
		currentPayloadParser->handleCharacterData(data);
	}
}

}
