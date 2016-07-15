/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/UserTuneSerializer.h>

#include <memory>

#include <boost/lexical_cast.hpp>

#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

UserTuneSerializer::UserTuneSerializer(PayloadSerializerCollection* /*serializers*/) {
}

UserTuneSerializer::~UserTuneSerializer() {
}

std::string UserTuneSerializer::serializePayload(std::shared_ptr<UserTune> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("tune", "http://jabber.org/protocol/tune");
    if (payload->getRating()) {
        element.addNode(std::make_shared<XMLElement>("rating", "", boost::lexical_cast<std::string>(*payload->getRating())));
    }
    if (payload->getTitle()) {
        element.addNode(std::make_shared<XMLElement>("title", "", *payload->getTitle()));
    }
    if (payload->getTrack()) {
        element.addNode(std::make_shared<XMLElement>("track", "", *payload->getTrack()));
    }
    if (payload->getArtist()) {
        element.addNode(std::make_shared<XMLElement>("artist", "", *payload->getArtist()));
    }
    if (payload->getURI()) {
        element.addNode(std::make_shared<XMLElement>("uri", "", *payload->getURI()));
    }
    if (payload->getSource()) {
        element.addNode(std::make_shared<XMLElement>("source", "", *payload->getSource()));
    }
    if (payload->getLength()) {
        element.addNode(std::make_shared<XMLElement>("length", "", boost::lexical_cast<std::string>(*payload->getLength())));
    }
    return element.serialize();
}


