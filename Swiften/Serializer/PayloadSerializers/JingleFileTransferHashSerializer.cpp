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

#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferHashSerializer.h>

#include <map>
#include <memory>
#include <string>

#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferFileInfoSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLNode.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

namespace Swift {

JingleFileTransferHashSerializer::JingleFileTransferHashSerializer() {
}

std::string JingleFileTransferHashSerializer::serializePayload(std::shared_ptr<JingleFileTransferHash> payload) const {
    // code for version urn:xmpp:jingle:apps:file-transfer:2
    //XMLElement hash("hash", "urn:xmpp:jingle:apps:file-transfer:info:2", payload->getHash());

    // code for version urn:xmpp:jingle:apps:file-transfer:4
    XMLElement checksum("checksum", "urn:xmpp:jingle:apps:file-transfer:4");

    JingleFileTransferFileInfoSerializer  fileSerializer;

    std::shared_ptr<XMLRawTextNode> file = std::make_shared<XMLRawTextNode>(fileSerializer.serialize(std::make_shared<JingleFileTransferFileInfo>(payload->getFileInfo())));

    checksum.addNode(file);

    return checksum.serialize();
}

}
