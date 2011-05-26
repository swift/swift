/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "JingleFileTransferDescriptionParser.h"

#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Base/Log.h>

namespace Swift {

JingleFileTransferDescriptionParser::JingleFileTransferDescriptionParser(PayloadParserFactoryCollection* factories) :   factories(factories), level(0),
															currentElement(UnknownElement) {
	
}

void JingleFileTransferDescriptionParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	if (level == 0) {
		
	}
	
	if (level == 1) {
		if (element == "offer") {
			currentElement = OfferElement;
		} else if (element == "request") {
			currentElement = RequestElement;
		} else {
			currentElement = UnknownElement;
		}
	}

	if (level == 2) {
		PayloadParserFactory* payloadParserFactory = factories->getPayloadParserFactory(element, ns, attributes);
		if (payloadParserFactory) {
			currentPayloadParser.reset(payloadParserFactory->createPayloadParser());
		}
	}

	if (level >= 2 && currentPayloadParser) {
		currentPayloadParser->handleStartElement(element, ns, attributes);
	}

	++level;
}

void JingleFileTransferDescriptionParser::handleEndElement(const std::string& element, const std::string& ns) {
	--level;
	if (currentPayloadParser) { 
		if (level >= 2) {
			currentPayloadParser->handleEndElement(element, ns);
		}

		if (level == 2) {
			boost::shared_ptr<StreamInitiationFileInfo> info = boost::dynamic_pointer_cast<StreamInitiationFileInfo>(currentPayloadParser->getPayload());
			if (info) {
				if (currentElement == OfferElement) {
					getPayloadInternal()->addOffer(*info);
				} else if (currentElement == RequestElement) {
					getPayloadInternal()->addRequest(*info);
				}
			}
		}
	}
}

void JingleFileTransferDescriptionParser::handleCharacterData(const std::string& data) {
	if (level >= 2 && currentPayloadParser) {
		currentPayloadParser->handleCharacterData(data);
	}
}
		
}
