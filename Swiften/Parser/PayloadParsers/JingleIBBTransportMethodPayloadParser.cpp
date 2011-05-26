/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include "JingleIBBTransportMethodPayloadParser.h"

#include <Swiften/Base/Log.h>

namespace Swift {
	JingleIBBTransportMethodPayloadParser::JingleIBBTransportMethodPayloadParser() : level(0) {
		
	}
	
	void JingleIBBTransportMethodPayloadParser::handleStartElement(const std::string&, const std::string&, const AttributeMap& attributes) {
		try {
			getPayloadInternal()->setBlockSize(boost::lexical_cast<int>(attributes.getAttributeValue("block-size").get_value_or("0")));
		} catch (boost::bad_lexical_cast &) {
			getPayloadInternal()->setBlockSize(0);
		}
		getPayloadInternal()->setSessionID(attributes.getAttributeValue("sid").get_value_or(""));
		++level;
	}
	
	void JingleIBBTransportMethodPayloadParser::handleEndElement(const std::string&, const std::string&) {
		--level;
		

	}
	
	void JingleIBBTransportMethodPayloadParser::handleCharacterData(const std::string&) {

	}
}
