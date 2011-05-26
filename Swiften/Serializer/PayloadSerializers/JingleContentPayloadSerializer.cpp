/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/JingleContentPayloadSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLNode.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferDescriptionSerializer.h>

#include <Swiften/Serializer/PayloadSerializers/JingleIBBTransportPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JingleS5BTransportPayloadSerializer.h>

#include "Swiften/FileTransfer/JingleTransport.h"

namespace Swift {

JingleContentPayloadSerializer::JingleContentPayloadSerializer() {
}

std::string JingleContentPayloadSerializer::serializePayload(boost::shared_ptr<JingleContentPayload> payload) const {
	XMLElement payloadXML("content");
	payloadXML.setAttribute("creator", creatorToString(payload->getCreator()));
	payloadXML.setAttribute("name", payload->getName());
	
	if (!payload->getDescriptions().empty()) {
		// JingleFileTransferDescription
		JingleFileTransferDescriptionSerializer ftSerializer;
		JingleFileTransferDescription::ref filetransfer;
		
		foreach(JingleDescription::ref desc, payload->getDescriptions()) {
			if ((filetransfer = boost::dynamic_pointer_cast<JingleFileTransferDescription>(desc))) {
				payloadXML.addNode(boost::make_shared<XMLRawTextNode>(ftSerializer.serializePayload(filetransfer)));
			}
		}
	}
	
	if (!payload->getTransports().empty()) {	
		// JingleIBBTransportPayload
		JingleIBBTransportPayloadSerializer ibbSerializer;
		JingleIBBTransportPayload::ref ibb;
		
		// JingleS5BTransportPayload
		JingleS5BTransportPayloadSerializer s5bSerializer;
		JingleS5BTransportPayload::ref s5b;

		foreach(JingleTransportPayload::ref transport, payload->getTransports()) {
			if ((ibb = boost::dynamic_pointer_cast<JingleIBBTransportPayload>(transport))) {
				payloadXML.addNode(boost::make_shared<XMLRawTextNode>(ibbSerializer.serializePayload(ibb)));
			} else if ((s5b = boost::dynamic_pointer_cast<JingleS5BTransportPayload>(transport))) {
				payloadXML.addNode(boost::make_shared<XMLRawTextNode>(s5bSerializer.serializePayload(s5b)));
			}
		}
	}
	return payloadXML.serialize();
}

std::string JingleContentPayloadSerializer::creatorToString(JingleContentPayload::Creator creator) const {
	switch(creator) {
		case JingleContentPayload::InitiatorCreator:
			return "initiator";
		case JingleContentPayload::ResponderCreator:
			return "responder";
		case JingleContentPayload::UnknownCreator:
			std::cerr << "Serializing unknown creator value." << std::endl;
			return "ERROR ERROR ERROR";
	}
	assert(false);
}
}
