/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferHashSerializer.h>

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLNode.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>


namespace Swift {

JingleFileTransferHashSerializer::JingleFileTransferHashSerializer() {
}

std::string JingleFileTransferHashSerializer::serializePayload(boost::shared_ptr<JingleFileTransferHash> payload) const {
	// code for version urn:xmpp:jingle:apps:file-transfer:2
	//XMLElement hash("hash", "urn:xmpp:jingle:apps:file-transfer:info:2", payload->getHash());

	// code for version urn:xmpp:jingle:apps:file-transfer:3
	XMLElement checksum("checksum", "urn:xmpp:jingle:apps:file-transfer:3");
	boost::shared_ptr<XMLElement> file = boost::make_shared<XMLElement>("file");
	checksum.addNode(file);
	boost::shared_ptr<XMLElement> hashes = boost::make_shared<XMLElement>("hashes", "urn:xmpp:hashes:0");
	file->addNode(hashes);
	foreach(const JingleFileTransferHash::HashesMap::value_type& pair, payload->getHashes()) {
		boost::shared_ptr<XMLElement> hash = boost::make_shared<XMLElement>("hash", "", pair.second);
		hash->setAttribute("algo", pair.first);
		hashes->addNode(hash);
	}

	return checksum.serialize();
}

}
