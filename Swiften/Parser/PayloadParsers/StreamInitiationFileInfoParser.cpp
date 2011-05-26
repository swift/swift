/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "StreamInitiationFileInfoParser.h"

#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Base/Log.h>

namespace Swift {

StreamInitiationFileInfoParser::StreamInitiationFileInfoParser() : level(0), parseDescription(false) {
	
}

void StreamInitiationFileInfoParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) {
	if (level == 0) {
		getPayloadInternal()->setName(attributes.getAttributeValue("name").get_value_or(""));
		getPayloadInternal()->setHash(attributes.getAttributeValue("hash").get_value_or(""));
		getPayloadInternal()->setAlgo(attributes.getAttributeValue("algo").get_value_or("md5"));
		try {
			getPayloadInternal()->setSize(boost::lexical_cast<boost::uintmax_t>(attributes.getAttributeValue("size").get_value_or("0")));
		} catch (boost::bad_lexical_cast &) {
			getPayloadInternal()->setSize(0);
		}
		getPayloadInternal()->setDate(stringToDateTime(attributes.getAttributeValue("date").get_value_or("")));
	} else if (level == 1) {
		if (element == "desc") {
			parseDescription = true;
		} else {
			parseDescription = false;
			if (element == "range") {
				int offset = 0;
				try {
					offset = boost::lexical_cast<boost::uintmax_t>(attributes.getAttributeValue("offset").get_value_or("0"));
				} catch (boost::bad_lexical_cast &) {
					offset = 0;
				}
				if (offset == 0) {
					getPayloadInternal()->setSupportsRangeRequests(true);
				} else {
					getPayloadInternal()->setRangeOffset(offset);
				}
			}
		}
	}
	++level;
}

void StreamInitiationFileInfoParser::handleEndElement(const std::string& element, const std::string&) {
	--level;
	if (parseDescription && element == "desc") {
		parseDescription = false;
		getPayloadInternal()->setDescription(desc);
	}
}

void StreamInitiationFileInfoParser::handleCharacterData(const std::string& data) {
	if (parseDescription) {
		desc += data;
	}
}

}
