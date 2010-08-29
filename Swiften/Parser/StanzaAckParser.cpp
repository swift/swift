/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/StanzaAckParser.h"

#include <boost/lexical_cast.hpp>

namespace Swift {

StanzaAckParser::StanzaAckParser() : GenericElementParser<StanzaAck>(), depth(0) {
}

void StanzaAckParser::handleStartElement(const String&, const String&, const AttributeMap& attributes) {
	if (depth == 0) {
		String handledStanzasString = attributes.getAttribute("h");
		try {
			getElementGeneric()->setHandledStanzasCount(boost::lexical_cast<int>(handledStanzasString.getUTF8String()));
		}
		catch (const boost::bad_lexical_cast &) {
		}
	}
	++depth;
}

void StanzaAckParser::handleEndElement(const String&, const String&) {
	--depth;
}

}
