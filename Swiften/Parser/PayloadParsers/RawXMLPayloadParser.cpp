/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/RawXMLPayloadParser.h>
#include <Swiften/Parser/SerializingParser.h>

namespace Swift {

RawXMLPayloadParser::RawXMLPayloadParser() : level_(0) {
}

void RawXMLPayloadParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	++level_;
	serializingParser_.handleStartElement(element, ns, attributes);
}

void RawXMLPayloadParser::handleEndElement(const std::string& element, const std::string& ns) {
	serializingParser_.handleEndElement(element, ns);
	--level_;
	if (level_ == 0) {
		getPayloadInternal()->setRawXML(serializingParser_.getResult());
	}
}

void RawXMLPayloadParser::handleCharacterData(const std::string& data) {
	serializingParser_.handleCharacterData(data);
}

}
