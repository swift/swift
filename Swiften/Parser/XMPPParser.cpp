#include "Swiften/Parser/XMPPParser.h"

#include <iostream>
#include <cassert>

#include "Swiften/Base/String.h"
#include "Swiften/Parser/XMLParser.h"
#include "Swiften/Parser/PlatformXMLParserFactory.h"
#include "Swiften/Parser/XMPPParserClient.h"
#include "Swiften/Parser/XMPPParser.h"
#include "Swiften/Parser/ElementParser.h"
#include "Swiften/Parser/PresenceParser.h"
#include "Swiften/Parser/IQParser.h"
#include "Swiften/Parser/MessageParser.h"
#include "Swiften/Parser/StreamFeaturesParser.h"
#include "Swiften/Parser/AuthRequestParser.h"
#include "Swiften/Parser/AuthSuccessParser.h"
#include "Swiften/Parser/AuthFailureParser.h"
#include "Swiften/Parser/StartTLSParser.h"
#include "Swiften/Parser/StartTLSFailureParser.h"
#include "Swiften/Parser/CompressParser.h"
#include "Swiften/Parser/CompressFailureParser.h"
#include "Swiften/Parser/CompressedParser.h"
#include "Swiften/Parser/UnknownElementParser.h"
#include "Swiften/Parser/TLSProceedParser.h"

// TODO: Whenever an error occurs in the handlers, stop the parser by returing
// a bool value, and stopping the XML parser

namespace Swift {

XMPPParser::XMPPParser(
		XMPPParserClient* client, 
		PayloadParserFactoryCollection* payloadParserFactories) : 
				xmlParser_(0),
				client_(client), 
				payloadParserFactories_(payloadParserFactories), 
				currentDepth_(0),
				currentElementParser_(0),
				parseErrorOccurred_(false) {
	xmlParser_ = PlatformXMLParserFactory().createXMLParser(this);
}

XMPPParser::~XMPPParser() {
	delete currentElementParser_;
	delete xmlParser_;
}

bool XMPPParser::parse(const String& data) {
	bool xmlParseResult = xmlParser_->parse(data);
	return xmlParseResult && !parseErrorOccurred_;
}

void XMPPParser::handleStartElement(const String& element, const String& ns, const AttributeMap& attributes) {
	if (!inStream()) {
		if (element == "stream" && ns == "http://etherx.jabber.org/streams") {
			client_->handleStreamStart(attributes.getAttribute("from"), attributes.getAttribute("to"), attributes.getAttribute("id"));
		}
		else {
			parseErrorOccurred_ = true;
		}
	}
	else {
		if (!inElement()) {
			assert(!currentElementParser_);
			delete currentElementParser_;
			currentElementParser_ = createElementParser(element, ns);
		}
		currentElementParser_->handleStartElement(element, ns, attributes);
	}
	++currentDepth_;
}

void XMPPParser::handleEndElement(const String& element, const String& ns) {
	assert(inStream());
	if (inElement()) {
		assert(currentElementParser_);
		currentElementParser_->handleEndElement(element, ns);
		--currentDepth_;
		if (!inElement()) {
			client_->handleElement(currentElementParser_->getElement());
			delete currentElementParser_;
			currentElementParser_ = 0;
		}
	}
	else {
		assert(element == "stream");
		--currentDepth_;
		client_->handleStreamEnd();
	}
}

void XMPPParser::handleCharacterData(const String& data) {
	if (currentElementParser_) {
		currentElementParser_->handleCharacterData(data);
	}
	//else {
	//	std::cerr << "XMPPParser: Ignoring stray character data: " << data << std::endl;
	//}
}

ElementParser* XMPPParser::createElementParser(const String& element, const String& ns) {
	if (element == "presence") {
		return new PresenceParser(payloadParserFactories_);
	}
	else if (element == "iq") {
		return new IQParser(payloadParserFactories_);
	}
	else if (element == "message") {
		return new MessageParser(payloadParserFactories_);
	}
	else if (element == "features"  && ns == "http://etherx.jabber.org/streams") {
		return new StreamFeaturesParser();
	}
	else if (element == "auth") {
		return new AuthRequestParser();
	}
	else if (element == "success") {
		return new AuthSuccessParser();
	}
	else if (element == "failure" && ns == "urn:ietf:params:xml:ns:xmpp-sasl") {
		return new AuthFailureParser();
	}
	else if (element == "starttls") {
		return new StartTLSParser();
	}
	else if (element == "failure" && ns == "urn:ietf:params:xml:ns:xmpp-tls") {
		return new StartTLSFailureParser();
	}
	else if (element == "compress") {
		return new CompressParser();
	}
	else if (element == "compressed") {
		return new CompressedParser();
	}
	else if (element == "failure" && ns == "http://jabber.org/protocol/compress") {
		return new CompressFailureParser();
	}
	else if (element == "proceed") {
		return new TLSProceedParser();
	}
	return new UnknownElementParser();
}

}
