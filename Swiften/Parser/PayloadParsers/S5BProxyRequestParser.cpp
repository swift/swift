/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "S5BProxyRequestParser.h"

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

namespace Swift {

S5BProxyRequestParser::S5BProxyRequestParser() : parseActivate(false) {
}

S5BProxyRequestParser::~S5BProxyRequestParser() {
}

void S5BProxyRequestParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) {
	if (element == "streamhost") {
		if (attributes.getAttributeValue("host") && attributes.getAttributeValue("jid") && attributes.getAttributeValue("port")) {
			HostAddress address = attributes.getAttributeValue("host").get_value_or("");
			int port = -1;
			JID jid = attributes.getAttributeValue("jid").get_value_or("");

			try {
				port = boost::lexical_cast<int>(attributes.getAttributeValue("port").get());
			} catch (boost::bad_lexical_cast &) {
				port = -1;
			}
			if (address.isValid() && port != -1 && jid.isValid()) {
				S5BProxyRequest::StreamHost streamHost;
				streamHost.addressPort = HostAddressPort(address, port);
				streamHost.jid = jid;
				getPayloadInternal()->setStreamHost(streamHost);
			}
		}
	} else if (element == "activate") {
		parseActivate = true;
	} else if (element == "query") {
		if (attributes.getAttributeValue("sid")) {
			getPayloadInternal()->setSID(attributes.getAttributeValue("sid").get());
		}
	}
}

void S5BProxyRequestParser::handleEndElement(const std::string& element, const std::string&) {
	if (element == "activate") {
		JID activate = JID(activateJID);
		if (activate.isValid()) {
			getPayloadInternal()->setActivate(activate);
		}
		parseActivate = false;
	}
}

void S5BProxyRequestParser::handleCharacterData(const std::string& data) {
	if (parseActivate) {
		activateJID = activateJID + data;
	}
}

}
