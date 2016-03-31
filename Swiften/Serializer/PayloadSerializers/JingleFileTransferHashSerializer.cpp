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
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferFileInfoSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLNode.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

namespace Swift {

JingleFileTransferHashSerializer::JingleFileTransferHashSerializer() {
}

std::string JingleFileTransferHashSerializer::serializePayload(boost::shared_ptr<JingleFileTransferHash> payload) const {
    // code for version urn:xmpp:jingle:apps:file-transfer:2
    //XMLElement hash("hash", "urn:xmpp:jingle:apps:file-transfer:info:2", payload->getHash());

    // code for version urn:xmpp:jingle:apps:file-transfer:4
    XMLElement checksum("checksum", "urn:xmpp:jingle:apps:file-transfer:4");

    JingleFileTransferFileInfoSerializer  fileSerializer;

    boost::shared_ptr<XMLRawTextNode> file = boost::make_shared<XMLRawTextNode>(fileSerializer.serialize(boost::make_shared<JingleFileTransferFileInfo>(payload->getFileInfo())));

    checksum.addNode(file);

    return checksum.serialize();
}

}
