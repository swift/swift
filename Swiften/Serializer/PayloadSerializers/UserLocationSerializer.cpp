/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/UserLocationSerializer.h>

#include <memory>

#include <boost/lexical_cast.hpp>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

UserLocationSerializer::UserLocationSerializer(PayloadSerializerCollection* /*serializers*/) {
}

UserLocationSerializer::~UserLocationSerializer() {
}

std::string UserLocationSerializer::serializePayload(std::shared_ptr<UserLocation> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("geoloc", "http://jabber.org/protocol/geoloc");
    if (payload->getArea()) {
        element.addNode(std::make_shared<XMLElement>("area", "", *payload->getArea()));
    }
    if (payload->getAltitude()) {
        element.addNode(std::make_shared<XMLElement>("alt", "", boost::lexical_cast<std::string>(*payload->getAltitude())));
    }
    if (payload->getLocality()) {
        element.addNode(std::make_shared<XMLElement>("locality", "", *payload->getLocality()));
    }
    if (payload->getLatitude()) {
        element.addNode(std::make_shared<XMLElement>("lat", "", boost::lexical_cast<std::string>(*payload->getLatitude())));
    }
    if (payload->getAccuracy()) {
        element.addNode(std::make_shared<XMLElement>("accuracy", "", boost::lexical_cast<std::string>(*payload->getAccuracy())));
    }
    if (payload->getDescription()) {
        element.addNode(std::make_shared<XMLElement>("description", "", *payload->getDescription()));
    }
    if (payload->getCountryCode()) {
        element.addNode(std::make_shared<XMLElement>("countrycode", "", *payload->getCountryCode()));
    }
    if (payload->getTimestamp()) {
        element.addNode(std::make_shared<XMLElement>("timestamp", "", dateTimeToString(*payload->getTimestamp())));
    }
    if (payload->getFloor()) {
        element.addNode(std::make_shared<XMLElement>("floor", "", *payload->getFloor()));
    }
    if (payload->getBuilding()) {
        element.addNode(std::make_shared<XMLElement>("building", "", *payload->getBuilding()));
    }
    if (payload->getRoom()) {
        element.addNode(std::make_shared<XMLElement>("room", "", *payload->getRoom()));
    }
    if (payload->getCountry()) {
        element.addNode(std::make_shared<XMLElement>("country", "", *payload->getCountry()));
    }
    if (payload->getRegion()) {
        element.addNode(std::make_shared<XMLElement>("region", "", *payload->getRegion()));
    }
    if (payload->getURI()) {
        element.addNode(std::make_shared<XMLElement>("uri", "", *payload->getURI()));
    }
    if (payload->getLongitude()) {
        element.addNode(std::make_shared<XMLElement>("lon", "", boost::lexical_cast<std::string>(*payload->getLongitude())));
    }
    if (payload->getError()) {
        element.addNode(std::make_shared<XMLElement>("error", "", boost::lexical_cast<std::string>(*payload->getError())));
    }
    if (payload->getPostalCode()) {
        element.addNode(std::make_shared<XMLElement>("postalcode", "", *payload->getPostalCode()));
    }
    if (payload->getBearing()) {
        element.addNode(std::make_shared<XMLElement>("bearing", "", boost::lexical_cast<std::string>(*payload->getBearing())));
    }
    if (payload->getText()) {
        element.addNode(std::make_shared<XMLElement>("text", "", *payload->getText()));
    }
    if (payload->getDatum()) {
        element.addNode(std::make_shared<XMLElement>("datum", "", *payload->getDatum()));
    }
    if (payload->getStreet()) {
        element.addNode(std::make_shared<XMLElement>("street", "", *payload->getStreet()));
    }
    if (payload->getSpeed()) {
        element.addNode(std::make_shared<XMLElement>("speed", "", boost::lexical_cast<std::string>(*payload->getSpeed())));
    }
    return element.serialize();
}


