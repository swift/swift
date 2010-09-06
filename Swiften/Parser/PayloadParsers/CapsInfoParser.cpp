/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/PayloadParsers/CapsInfoParser.h"

#include <locale>

#include <boost/date_time/time_facet.hpp>

namespace Swift {

CapsInfoParser::CapsInfoParser() : level(0) {
}

void CapsInfoParser::handleStartElement(const String&, const String& /*ns*/, const AttributeMap& attributes) {
	if (level == 0) {
		getPayloadInternal()->setHash(attributes.getAttribute("hash"));
		getPayloadInternal()->setNode(attributes.getAttribute("node"));
		getPayloadInternal()->setVersion(attributes.getAttribute("ver"));
	}
	++level;
}

void CapsInfoParser::handleEndElement(const String&, const String&) {
	--level;
}

void CapsInfoParser::handleCharacterData(const String&) {

}

}
