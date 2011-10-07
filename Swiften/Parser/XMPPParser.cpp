/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/XMPPParser.h>

#include <iostream>
#include <cassert>

#include <Swiften/Elements/ProtocolHeader.h>
#include <string>
#include <Swiften/Parser/XMLParser.h>
#include <Swiften/Parser/XMPPParserClient.h>
#include <Swiften/Parser/XMPPParser.h>
#include <Swiften/Parser/ElementParser.h>
#include <Swiften/Parser/PresenceParser.h>
#include <Swiften/Parser/IQParser.h>
#include <Swiften/Parser/MessageParser.h>
#include <Swiften/Parser/StreamFeaturesParser.h>
#include <Swiften/Parser/StreamErrorParser.h>
#include <Swiften/Parser/AuthRequestParser.h>
#include <Swiften/Parser/AuthSuccessParser.h>
#include <Swiften/Parser/AuthFailureParser.h>
#include <Swiften/Parser/AuthChallengeParser.h>
#include <Swiften/Parser/AuthResponseParser.h>
#include <Swiften/Parser/EnableStreamManagementParser.h>
#include <Swiften/Parser/StreamManagementEnabledParser.h>
#include <Swiften/Parser/StreamManagementFailedParser.h>
#include <Swiften/Parser/StreamResumeParser.h>
#include <Swiften/Parser/StreamResumedParser.h>
#include <Swiften/Parser/StanzaAckParser.h>
#include <Swiften/Parser/StanzaAckRequestParser.h>
#include <Swiften/Parser/StartTLSParser.h>
#include <Swiften/Parser/StartTLSFailureParser.h>
#include <Swiften/Parser/CompressParser.h>
#include <Swiften/Parser/CompressFailureParser.h>
#include <Swiften/Parser/CompressedParser.h>
#include <Swiften/Parser/UnknownElementParser.h>
#include <Swiften/Parser/TLSProceedParser.h>
#include <Swiften/Parser/ComponentHandshakeParser.h>
#include <Swiften/Parser/XMLParserFactory.h>

// TODO: Whenever an error occurs in the handlers, stop the parser by returing
// a bool value, and stopping the XML parser

namespace Swift {

XMPPParser::XMPPParser(
		XMPPParserClient* client, 
		PayloadParserFactoryCollection* payloadParserFactories,
		XMLParserFactory* xmlParserFactory) : 
				xmlParser_(0),
				client_(client), 
				payloadParserFactories_(payloadParserFactories), 
				level_(0),
				currentElementParser_(0),
				parseErrorOccurred_(false) {
	xmlParser_ = xmlParserFactory->createXMLParser(this);
}

XMPPParser::~XMPPParser() {
	delete currentElementParser_;
	delete xmlParser_;
}

bool XMPPParser::parse(const std::string& data) {
	bool xmlParseResult = xmlParser_->parse(data);
	return xmlParseResult && !parseErrorOccurred_;
}

void XMPPParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	if (!parseErrorOccurred_) {
		if (level_ == TopLevel) {
			if (element == "stream" && ns == "http://etherx.jabber.org/streams") {
				ProtocolHeader header;
				header.setFrom(attributes.getAttribute("from"));
				header.setTo(attributes.getAttribute("to"));
				header.setID(attributes.getAttribute("id"));
				header.setVersion(attributes.getAttribute("version"));
				client_->handleStreamStart(header);
			}
			else {
				parseErrorOccurred_ = true;
			}
		}
		else {
			if (level_ == StreamLevel) {
				assert(!currentElementParser_);
				currentElementParser_ = createElementParser(element, ns);
			}
			currentElementParser_->handleStartElement(element, ns, attributes);
		}
	}
	++level_;
}

void XMPPParser::handleEndElement(const std::string& element, const std::string& ns) {
	assert(level_ > TopLevel);
	--level_;
	if (!parseErrorOccurred_) {
		if (level_ == TopLevel) {
			assert(element == "stream");
			client_->handleStreamEnd();
		}
		else {
			assert(currentElementParser_);
			currentElementParser_->handleEndElement(element, ns);
			if (level_ == StreamLevel) {
				client_->handleElement(currentElementParser_->getElement());
				delete currentElementParser_;
				currentElementParser_ = NULL;
			}
		}
	}
}

void XMPPParser::handleCharacterData(const std::string& data) {
	if (!parseErrorOccurred_) {
		if (currentElementParser_) {
			currentElementParser_->handleCharacterData(data);
		}
	//else {
	//	std::cerr << "XMPPParser: Ignoring stray character data: " << data << std::endl;
	//}
	}
}

ElementParser* XMPPParser::createElementParser(const std::string& element, const std::string& ns) {
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
	else if (element == "error" && ns == "http://etherx.jabber.org/streams") {
		return new StreamErrorParser();
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
	else if (element == "challenge" && ns == "urn:ietf:params:xml:ns:xmpp-sasl") {
		return new AuthChallengeParser();
	}
	else if (element == "response" && ns == "urn:ietf:params:xml:ns:xmpp-sasl") {
		return new AuthResponseParser();
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
	else if (element == "enable" && ns == "urn:xmpp:sm:2") {
		return new EnableStreamManagementParser();
	}
	else if (element == "enabled" && ns == "urn:xmpp:sm:2") {
		return new StreamManagementEnabledParser();
	}
	else if (element == "failed" && ns == "urn:xmpp:sm:2") {
		return new StreamManagementFailedParser();
	}
	else if (element == "resume" && ns == "urn:xmpp:sm:2") {
		return new StreamResumeParser();
	}
	else if (element == "resumed" && ns == "urn:xmpp:sm:2") {
		return new StreamResumedParser();
	}
	else if (element == "a" && ns == "urn:xmpp:sm:2") {
		return new StanzaAckParser();
	}
	else if (element == "r" && ns == "urn:xmpp:sm:2") {
		return new StanzaAckRequestParser();
	}
	else if (element == "handshake") {
		return new ComponentHandshakeParser();
	}
	return new UnknownElementParser();
}

}
