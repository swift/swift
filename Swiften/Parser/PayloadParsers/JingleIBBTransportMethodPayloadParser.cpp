/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/JingleIBBTransportMethodPayloadParser.h>

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/Log.h>

namespace Swift {
    JingleIBBTransportMethodPayloadParser::JingleIBBTransportMethodPayloadParser() : level(0) {

    }

    void JingleIBBTransportMethodPayloadParser::handleStartElement(const std::string&, const std::string&, const AttributeMap& attributes) {
        try {
            boost::optional<std::string> blockSize = attributes.getAttributeValue("block-size");
            if (blockSize) {
                getPayloadInternal()->setBlockSize(boost::lexical_cast<unsigned int>(*blockSize));
            }
        }
        catch (boost::bad_lexical_cast &) {
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
