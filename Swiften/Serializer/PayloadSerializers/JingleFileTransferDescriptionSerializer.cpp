/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferDescriptionSerializer.h>

#include <memory>

#include <boost/lexical_cast.hpp>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferFileInfoSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLNode.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/StringCodecs/Base64.h>

namespace Swift {

JingleFileTransferDescriptionSerializer::JingleFileTransferDescriptionSerializer() {
}

std::string JingleFileTransferDescriptionSerializer::serializePayload(std::shared_ptr<JingleFileTransferDescription> payload) const {
    XMLElement description("description", "urn:xmpp:jingle:apps:file-transfer:4");

    JingleFileTransferFileInfoSerializer fileInfoSerializer;
    std::shared_ptr<XMLRawTextNode> fileInfoXML = std::make_shared<XMLRawTextNode>(fileInfoSerializer.serialize(std::make_shared<JingleFileTransferFileInfo>(payload->getFileInfo())));
    description.addNode(fileInfoXML);
    return description.serialize();
}

}
