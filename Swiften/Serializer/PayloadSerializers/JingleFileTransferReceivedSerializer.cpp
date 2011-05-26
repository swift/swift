/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferReceivedSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLNode.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

#include <Swiften/Serializer/XML/XMLRawTextNode.h>

namespace Swift {

JingleFileTransferReceivedSerializer::JingleFileTransferReceivedSerializer() {
}

std::string JingleFileTransferReceivedSerializer::serializePayload(boost::shared_ptr<JingleFileTransferReceived> payload) const {
	XMLElement receivedElement("received", "urn:xmpp:jingle:apps:file-transfer:3");
	XMLElement::ref fileElement = boost::make_shared<XMLElement>("file", "http://jabber.org/protocol/si/profile/file-transfer");
	fileElement->setAttribute("hash", payload->getFileInfo().getHash());
	if (payload->getFileInfo().getAlgo() != "md5") {
		fileElement->setAttribute("algo", payload->getFileInfo().getAlgo());
	}
	receivedElement.addNode(fileElement);
	return receivedElement.serialize();
}

}
