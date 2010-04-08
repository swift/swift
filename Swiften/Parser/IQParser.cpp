/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <iostream>

#include "Swiften/Parser/IQParser.h"

namespace Swift {

IQParser::IQParser(PayloadParserFactoryCollection* factories) : 
		GenericStanzaParser<IQ>(factories) {
}

void IQParser::handleStanzaAttributes(const AttributeMap& attributes) {
	AttributeMap::const_iterator type = attributes.find("type");
	if (type != attributes.end()) {
		if (type->second == "set") {
			getStanzaGeneric()->setType(IQ::Set);
		}
		else if (type->second == "get") {
			getStanzaGeneric()->setType(IQ::Get);
		}
		else if (type->second == "result") {
			getStanzaGeneric()->setType(IQ::Result);
		}
		else if (type->second == "error") {
			getStanzaGeneric()->setType(IQ::Error);
		}
		else {
			std::cerr << "Unknown IQ type: " << type->second << std::endl;
			getStanzaGeneric()->setType(IQ::Get);
		}
	}
}

}
