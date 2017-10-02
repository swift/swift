/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Entity/Entity.h>

#include <Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

namespace Swift {

Entity::Entity() {
    payloadParserFactories = new FullPayloadParserFactoryCollection();
    payloadSerializers = new FullPayloadSerializerCollection();
}

Entity::~Entity() {
    delete payloadSerializers;
    delete payloadParserFactories;
}


void Entity::addPayloadParserFactory(PayloadParserFactory* payloadParserFactory) {
    payloadParserFactories->addFactory(payloadParserFactory);
}

void Entity::removePayloadParserFactory(PayloadParserFactory* payloadParserFactory) {
    payloadParserFactories->removeFactory(payloadParserFactory);
}

void Entity::addPayloadSerializer(PayloadSerializer* payloadSerializer) {
    payloadSerializers->addSerializer(payloadSerializer);
}

void Entity::removePayloadSerializer(PayloadSerializer* payloadSerializer) {
    payloadSerializers->removeSerializer(payloadSerializer);
}

PayloadParserFactoryCollection* Entity::getPayloadParserFactories() {
    return payloadParserFactories;
}

PayloadSerializerCollection* Entity::getPayloadSerializers() {
    return payloadSerializers;
}

}
