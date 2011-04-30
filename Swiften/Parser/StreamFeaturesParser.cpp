/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/StreamFeaturesParser.h>

namespace Swift {

StreamFeaturesParser::StreamFeaturesParser() : GenericElementParser<StreamFeatures>(), currentDepth_(0), inMechanisms_(false), inMechanism_(false), inCompression_(false), inCompressionMethod_(false) {
}

void StreamFeaturesParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap&) {
	if (currentDepth_ == 1) {
		if (element == "starttls" && ns == "urn:ietf:params:xml:ns:xmpp-tls") {
			getElementGeneric()->setHasStartTLS();
		}
		else if (element == "session" && ns == "urn:ietf:params:xml:ns:xmpp-session") {
			getElementGeneric()->setHasSession();
		}
		else if (element == "bind" && ns == "urn:ietf:params:xml:ns:xmpp-bind") {
			getElementGeneric()->setHasResourceBind();
		}
		else if (element == "sm" && ns == "urn:xmpp:sm:2") {
			getElementGeneric()->setHasStreamManagement();
		}
		else if (element == "mechanisms" && ns == "urn:ietf:params:xml:ns:xmpp-sasl") {
			inMechanisms_ = true;
		}
		else if (element == "compression" && ns == "http://jabber.org/features/compress") {
			inCompression_ = true;
		}
		else if (element == "ver" && ns == "urn:xmpp:features:rosterver") {
			getElementGeneric()->setHasRosterVersioning();
		}
	}
	else if (currentDepth_ == 2) {
		if (inCompression_ && element == "method") {
			inCompressionMethod_ = true;
			currentText_ = "";
		}
		else if (inMechanisms_ && element == "mechanism") {
			inMechanism_ = true;
			currentText_ = "";
		}
	}
	++currentDepth_;
}

void StreamFeaturesParser::handleEndElement(const std::string&, const std::string&) {
	--currentDepth_;
	if (currentDepth_ == 1) {
		inCompression_ = false;
		inMechanisms_ = false;
	}
	else if (currentDepth_ == 2) {
		if (inCompressionMethod_) {
			getElementGeneric()->addCompressionMethod(currentText_);
			inCompressionMethod_ = false;
		}
		else if (inMechanism_) {
			getElementGeneric()->addAuthenticationMechanism(currentText_);
			inMechanism_ = false;
		}
	}
}

void StreamFeaturesParser::handleCharacterData(const std::string& data) {
	currentText_ += data;
}

}
