/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Entity/Entity.h"

namespace Swift {

Entity::~Entity() {
}


void Entity::addPayloadParserFactory(PayloadParserFactory* payloadParserFactory) {
	payloadParserFactories.addFactory(payloadParserFactory);
}

void Entity::removePayloadParserFactory(PayloadParserFactory* payloadParserFactory) {
	payloadParserFactories.removeFactory(payloadParserFactory);
}

void Entity::addPayloadSerializer(PayloadSerializer* payloadSerializer) {
	payloadSerializers.addSerializer(payloadSerializer);
}

void Entity::removePayloadSerializer(PayloadSerializer* payloadSerializer) {
	payloadSerializers.removeSerializer(payloadSerializer);
}

}
