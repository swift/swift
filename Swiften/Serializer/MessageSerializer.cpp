/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/MessageSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {

MessageSerializer::MessageSerializer(PayloadSerializerCollection* payloadSerializers) : 
		GenericStanzaSerializer<Message>("message", payloadSerializers) {
}

void MessageSerializer::setStanzaSpecificAttributesGeneric(
		boost::shared_ptr<Message> message, 
		XMLElement& element) const {
	if (message->getType() == Message::Chat) {
		element.setAttribute("type", "chat");
	}
	else if (message->getType() == Message::Groupchat) {
		element.setAttribute("type", "groupchat");
	}
	else if (message->getType() == Message::Headline) {
		element.setAttribute("type", "headline");
	}
	else if (message->getType() == Message::Error) {
		element.setAttribute("type", "error");
	}
}

}
