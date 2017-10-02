/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/UserTuneParser.h>

#include <boost/lexical_cast.hpp>

using namespace Swift;

UserTuneParser::UserTuneParser() : level(0) {
}

UserTuneParser::~UserTuneParser() {
}

void UserTuneParser::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
    if (level == 1) {
        currentText = "";
    }
    ++level;
}

void UserTuneParser::handleEndElement(const std::string& element, const std::string&) {
    --level;
    if (level == 1) {
        try {
            if (element == "artist") {
                getPayloadInternal()->setArtist(currentText);
            }
            else if (element == "length") {
                getPayloadInternal()->setLength(boost::lexical_cast<unsigned int>(currentText));
            }
            else if (element == "rating") {
                getPayloadInternal()->setRating(boost::lexical_cast<unsigned int>(currentText));
            }
            else if (element == "source") {
                getPayloadInternal()->setSource(currentText);
            }
            else if (element == "title") {
                getPayloadInternal()->setTitle(currentText);
            }
            else if (element == "track") {
                getPayloadInternal()->setTrack(currentText);
            }
            else if (element == "URI") {
                getPayloadInternal()->setURI(currentText);
            }
        }
        catch (const boost::bad_lexical_cast&) {
        }
    }
}

void UserTuneParser::handleCharacterData(const std::string& data) {
    currentText += data;
}
