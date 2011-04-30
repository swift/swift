/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/CapsInfoSerializer.h>

#include <boost/shared_ptr.hpp>

#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {

CapsInfoSerializer::CapsInfoSerializer() : GenericPayloadSerializer<CapsInfo>() {
}

std::string CapsInfoSerializer::serializePayload(boost::shared_ptr<CapsInfo> capsInfo)  const {
	XMLElement capsElement("c", "http://jabber.org/protocol/caps");
	capsElement.setAttribute("node", capsInfo->getNode());
	capsElement.setAttribute("hash", capsInfo->getHash());
	capsElement.setAttribute("ver", capsInfo->getVersion());
	return capsElement.serialize();
}

}
