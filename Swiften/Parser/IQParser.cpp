/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <iostream>
#include <boost/optional.hpp>

#include <Swiften/Parser/IQParser.h>

namespace Swift {

IQParser::IQParser(PayloadParserFactoryCollection* factories) : 
		GenericStanzaParser<IQ>(factories) {
}

void IQParser::handleStanzaAttributes(const AttributeMap& attributes) {
	boost::optional<std::string> type = attributes.getAttributeValue("type");
	if (type) {
		if (*type == "set") {
			getStanzaGeneric()->setType(IQ::Set);
		}
		else if (*type == "get") {
			getStanzaGeneric()->setType(IQ::Get);
		}
		else if (*type == "result") {
			getStanzaGeneric()->setType(IQ::Result);
		}
		else if (*type == "error") {
			getStanzaGeneric()->setType(IQ::Error);
		}
		else {
			std::cerr << "Unknown IQ type: " << *type << std::endl;
			getStanzaGeneric()->setType(IQ::Get);
		}
	}
}

}
