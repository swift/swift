/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/UserLocationParser.h>

#include <boost/lexical_cast.hpp>

#include <Swiften/Base/DateTime.h>

using namespace Swift;

UserLocationParser::UserLocationParser() : level(0) {
}

UserLocationParser::~UserLocationParser() {
}

void UserLocationParser::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
    if (level == 1) {
        currentText = "";
    }
    ++level;
}

void UserLocationParser::handleEndElement(const std::string& element, const std::string&) {
    --level;
    if (level == 1) {
        try {
            if (element == "accuracy") {
                getPayloadInternal()->setAccuracy(boost::lexical_cast<double>(currentText));
            }
            else if (element == "alt") {
                getPayloadInternal()->setAltitude(boost::lexical_cast<double>(currentText));
            }
            else if (element == "area") {
                getPayloadInternal()->setArea(currentText);
            }
            else if (element == "bearing") {
                getPayloadInternal()->setBearing(boost::lexical_cast<double>(currentText));
            }
            else if (element == "building") {
                getPayloadInternal()->setBuilding(currentText);
            }
            else if (element == "country") {
                getPayloadInternal()->setCountry(currentText);
            }
            else if (element == "countrycode") {
                getPayloadInternal()->setCountryCode(currentText);
            }
            else if (element == "datum") {
                getPayloadInternal()->setDatum(currentText);
            }
            else if (element == "description") {
                getPayloadInternal()->setDescription(currentText);
            }
            else if (element == "error") {
                getPayloadInternal()->setError(boost::lexical_cast<double>(currentText));
            }
            else if (element == "floor") {
                getPayloadInternal()->setFloor(currentText);
            }
            else if (element == "lat") {
                getPayloadInternal()->setLatitude(boost::lexical_cast<double>(currentText));
            }
            else if (element == "locality") {
                getPayloadInternal()->setLocality(currentText);
            }
            else if (element == "lon") {
                getPayloadInternal()->setLongitude(boost::lexical_cast<double>(currentText));
            }
            else if (element == "postalcode") {
                getPayloadInternal()->setPostalCode(currentText);
            }
            else if (element == "region") {
                getPayloadInternal()->setRegion(currentText);
            }
            else if (element == "room") {
                getPayloadInternal()->setRoom(currentText);
            }
            else if (element == "speed") {
                getPayloadInternal()->setSpeed(boost::lexical_cast<double>(currentText));
            }
            else if (element == "street") {
                getPayloadInternal()->setStreet(currentText);
            }
            else if (element == "text") {
                getPayloadInternal()->setText(currentText);
            }
            else if (element == "timestamp") {
                getPayloadInternal()->setTimestamp(stringToDateTime(currentText));
            }
            else if (element == "uri") {
                getPayloadInternal()->setURI(currentText);
            }
        }
        catch (const boost::bad_lexical_cast&) {
        }
    }
}

void UserLocationParser::handleCharacterData(const std::string& data) {
    currentText += data;
}
