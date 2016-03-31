/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/JingleFileTransferFileInfoParser.h>

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/DateTime.h>
#include <Swiften/StringCodecs/Base64.h>

namespace Swift {

JingleFileTransferFileInfoParser::JingleFileTransferFileInfoParser() : level(0) {
}

template<typename T> boost::optional<T> safeLexicalCast(const std::string& str) {
    boost::optional<T> ret;
    try {
        ret = boost::lexical_cast<T>(str);
    } catch (boost::bad_lexical_cast &) {

    }
    return ret;
}

void JingleFileTransferFileInfoParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) {
    charData.clear();
    if (element == "hash") {
        hashAlg = attributes.getAttributeValue("algo").get_value_or("");
    }
    else if (element == "range") {
        rangeOffset = safeLexicalCast<boost::uintmax_t>(attributes.getAttributeValue("offset").get_value_or(""));
    }

    ++level;
}

void JingleFileTransferFileInfoParser::handleEndElement(const std::string& element, const std::string&) {
    --level;
    if (level == 1) {
        if (element == "date") {
            getPayloadInternal()->setDate(stringToDateTime(charData));
        }
        else if (element == "desc") {
            getPayloadInternal()->setDescription(charData);
        }
        else if (element == "media-type") {
            getPayloadInternal()->setMediaType(charData);
        }
        else if (element == "name") {
            getPayloadInternal()->setName(charData);
        }
        else if (element == "size") {
            boost::optional<boost::uintmax_t> size = safeLexicalCast<boost::uintmax_t>(charData);
            if (size) {
                getPayloadInternal()->setSize(size.get());
            }
        }
        else if (element == "range") {
            getPayloadInternal()->setSupportsRangeRequests(true);
            if (rangeOffset) {
                getPayloadInternal()->setRangeOffset(rangeOffset.get_value_or(0));
            }
        }
        else if (element == "hash") {
            getPayloadInternal()->addHash(HashElement(hashAlg, Base64::decode(charData)));
        }
    }
}

void JingleFileTransferFileInfoParser::handleCharacterData(const std::string& data) {
    charData += data;
}

}
