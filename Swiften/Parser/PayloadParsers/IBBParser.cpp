/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/IBBParser.h>

#include <boost/lexical_cast.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/StringCodecs/Base64.h>

namespace Swift {

IBBParser::IBBParser() : level(TopLevel) {
}

IBBParser::~IBBParser() {
}

void IBBParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) {
	if (level == TopLevel) {
		if (element == "data") {
			getPayloadInternal()->setAction(IBB::Data);
			getPayloadInternal()->setStreamID(attributes.getAttribute("sid"));
			try {
				getPayloadInternal()->setSequenceNumber(boost::lexical_cast<int>(attributes.getAttribute("seq")));
			}
			catch (boost::bad_lexical_cast&) {
			}
		}
		else if (element == "open") {
			getPayloadInternal()->setAction(IBB::Open);
			getPayloadInternal()->setStreamID(attributes.getAttribute("sid"));
			if (attributes.getAttribute("stanza") == "message") {
				getPayloadInternal()->setStanzaType(IBB::MessageStanza);
			}
			else {
				getPayloadInternal()->setStanzaType(IBB::IQStanza);
			}
			try {
				getPayloadInternal()->setBlockSize(boost::lexical_cast<int>(attributes.getAttribute("block-size")));
			}
			catch (boost::bad_lexical_cast&) {
			}
		}
		else if (element == "close") {
			getPayloadInternal()->setAction(IBB::Close);
			getPayloadInternal()->setStreamID(attributes.getAttribute("sid"));
		}
	}
	++level;
}

void IBBParser::handleEndElement(const std::string& element, const std::string&) {
	--level;
	if (level == TopLevel) {
		if (element == "data") {
			std::vector<char> data;
			for (size_t i = 0; i < currentText.size(); ++i) {
				char c = currentText[i];
				if ((c >= 48 && c <= 122) || c == 47 || c == 43) {
					data.push_back(c);
				}
			}
			getPayloadInternal()->setData(Base64::decode(std::string(&data[0], data.size())));
		}
	}
}

void IBBParser::handleCharacterData(const std::string& data) {
	currentText += data;
}


}
