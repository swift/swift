/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/NicknameSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

NicknameSerializer::NicknameSerializer() : GenericPayloadSerializer<Nickname>() {
}

std::string NicknameSerializer::serializePayload(boost::shared_ptr<Nickname> nick)  const {
	XMLElement nickElement("nick", "http://jabber.org/protocol/nick");
	nickElement.addNode(boost::make_shared<XMLTextNode>(nick->getNickname()));
	return nickElement.serialize();
}

}
