/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "JingleFileTransferHashParser.h"

#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>

#include <Swiften/Parser/PayloadParsers/StreamInitiationFileInfoParser.h>
#include <Swiften/Parser/GenericPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactory.h>

namespace Swift {

JingleFileTransferHashParser::JingleFileTransferHashParser() {
}
	
void JingleFileTransferHashParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) {
	if (element == "hash") {
		algo = attributes.getAttribute("algo");
	}
}

void JingleFileTransferHashParser::handleEndElement(const std::string& element, const std::string& ) {
	if (element == "hash" && !algo.empty() && !hash.empty()) {
		getPayloadInternal()->setHash(algo, hash);
		algo.clear();
		hash.clear();
	}
}

void JingleFileTransferHashParser::handleCharacterData(const std::string& data) {
	if (!algo.empty()) {
		std::string new_data(data);
		boost::trim(new_data);
		hash += new_data;
	}
}

}
