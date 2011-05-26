/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "JingleFileTransferReceivedParser.h"
#include "StreamInitiationFileInfoParser.h"

#include <boost/shared_ptr.hpp>
#include <Swiften/Parser/PayloadParsers/StreamInitiationFileInfoParser.h>
#include <Swiften/Parser/GenericPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactory.h>

namespace Swift {

JingleFileTransferReceivedParser::JingleFileTransferReceivedParser() : level(0) {
}
	
void JingleFileTransferReceivedParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	if (level == 1 && element == "file") {
		PayloadParserFactory* payloadParserFactory = new GenericPayloadParserFactory<StreamInitiationFileInfoParser>("file", "http://jabber.org/protocol/si/profile/file-transfer");
		if (payloadParserFactory) {
			currentPayloadParser.reset(payloadParserFactory->createPayloadParser());
		}
	}
	
	if (currentPayloadParser && level >= 1) {
		currentPayloadParser->handleStartElement(element, ns, attributes);
	}
	
	++level;
}

void JingleFileTransferReceivedParser::handleEndElement(const std::string& element, const std::string& ) {
	--level;
	if (element == "file") {
		boost::shared_ptr<StreamInitiationFileInfo> fileInfo = boost::dynamic_pointer_cast<StreamInitiationFileInfo>(currentPayloadParser->getPayload());
		if (fileInfo) {
			getPayloadInternal()->setFileInfo(*fileInfo);
		}
	}
}

void JingleFileTransferReceivedParser::handleCharacterData(const std::string& ) {
	
}

}
