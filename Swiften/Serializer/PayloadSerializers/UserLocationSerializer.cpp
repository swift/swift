/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/UserLocationSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Base/DateTime.h>
#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

UserLocationSerializer::UserLocationSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

UserLocationSerializer::~UserLocationSerializer() {
}

std::string UserLocationSerializer::serializePayload(boost::shared_ptr<UserLocation> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("geoloc", "http://jabber.org/protocol/geoloc");
    if (payload->getArea()) {
        element.addNode(boost::make_shared<XMLElement>("area", "", *payload->getArea()));
    }
    if (payload->getAltitude()) {
        element.addNode(boost::make_shared<XMLElement>("alt", "", boost::lexical_cast<std::string>(*payload->getAltitude())));
    }
    if (payload->getLocality()) {
        element.addNode(boost::make_shared<XMLElement>("locality", "", *payload->getLocality()));
    }
    if (payload->getLatitude()) {
        element.addNode(boost::make_shared<XMLElement>("lat", "", boost::lexical_cast<std::string>(*payload->getLatitude())));
    }
    if (payload->getAccuracy()) {
        element.addNode(boost::make_shared<XMLElement>("accuracy", "", boost::lexical_cast<std::string>(*payload->getAccuracy())));
    }
    if (payload->getDescription()) {
        element.addNode(boost::make_shared<XMLElement>("description", "", *payload->getDescription()));
    }
    if (payload->getCountryCode()) {
        element.addNode(boost::make_shared<XMLElement>("countrycode", "", *payload->getCountryCode()));
    }
    if (payload->getTimestamp()) {
        element.addNode(boost::make_shared<XMLElement>("timestamp", "", dateTimeToString(*payload->getTimestamp())));
    }
    if (payload->getFloor()) {
        element.addNode(boost::make_shared<XMLElement>("floor", "", *payload->getFloor()));
    }
    if (payload->getBuilding()) {
        element.addNode(boost::make_shared<XMLElement>("building", "", *payload->getBuilding()));
    }
    if (payload->getRoom()) {
        element.addNode(boost::make_shared<XMLElement>("room", "", *payload->getRoom()));
    }
    if (payload->getCountry()) {
        element.addNode(boost::make_shared<XMLElement>("country", "", *payload->getCountry()));
    }
    if (payload->getRegion()) {
        element.addNode(boost::make_shared<XMLElement>("region", "", *payload->getRegion()));
    }
    if (payload->getURI()) {
        element.addNode(boost::make_shared<XMLElement>("uri", "", *payload->getURI()));
    }
    if (payload->getLongitude()) {
        element.addNode(boost::make_shared<XMLElement>("lon", "", boost::lexical_cast<std::string>(*payload->getLongitude())));
    }
    if (payload->getError()) {
        element.addNode(boost::make_shared<XMLElement>("error", "", boost::lexical_cast<std::string>(*payload->getError())));
    }
    if (payload->getPostalCode()) {
        element.addNode(boost::make_shared<XMLElement>("postalcode", "", *payload->getPostalCode()));
    }
    if (payload->getBearing()) {
        element.addNode(boost::make_shared<XMLElement>("bearing", "", boost::lexical_cast<std::string>(*payload->getBearing())));
    }
    if (payload->getText()) {
        element.addNode(boost::make_shared<XMLElement>("text", "", *payload->getText()));
    }
    if (payload->getDatum()) {
        element.addNode(boost::make_shared<XMLElement>("datum", "", *payload->getDatum()));
    }
    if (payload->getStreet()) {
        element.addNode(boost::make_shared<XMLElement>("street", "", *payload->getStreet()));
    }
    if (payload->getSpeed()) {
        element.addNode(boost::make_shared<XMLElement>("speed", "", boost::lexical_cast<std::string>(*payload->getSpeed())));
    }
    return element.serialize();
}


