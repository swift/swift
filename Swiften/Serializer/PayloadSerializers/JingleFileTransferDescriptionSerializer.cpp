/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferDescriptionSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLNode.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

#include <Swiften/Serializer/PayloadSerializers/StreamInitiationFileInfoSerializer.h>

namespace Swift {

JingleFileTransferDescriptionSerializer::JingleFileTransferDescriptionSerializer() {
}

std::string JingleFileTransferDescriptionSerializer::serializePayload(boost::shared_ptr<JingleFileTransferDescription> payload) const {
	XMLElement description("description", "urn:xmpp:jingle:apps:file-transfer:3");
	StreamInitiationFileInfoSerializer fileInfoSerializer;
	if (!payload->getOffers().empty()) {
		boost::shared_ptr<XMLElement> offers = boost::make_shared<XMLElement>("offer");
		foreach(const StreamInitiationFileInfo &fileInfo, payload->getOffers()) {
			boost::shared_ptr<XMLRawTextNode> fileInfoXML = boost::make_shared<XMLRawTextNode>(fileInfoSerializer.serialize(boost::make_shared<StreamInitiationFileInfo>(fileInfo)));
			offers->addNode(fileInfoXML);
		}
		description.addNode(offers);
	}
	if (!payload->getRequests().empty()) {
		boost::shared_ptr<XMLElement> requests = boost::make_shared<XMLElement>("request");
		foreach(const StreamInitiationFileInfo &fileInfo, payload->getRequests()) {
			boost::shared_ptr<XMLRawTextNode> fileInfoXML = boost::make_shared<XMLRawTextNode>(fileInfoSerializer.serialize(boost::make_shared<StreamInitiationFileInfo>(fileInfo)));
			requests->addNode(fileInfoXML);
		}
		description.addNode(requests);
	}
	return description.serialize();
}

}
