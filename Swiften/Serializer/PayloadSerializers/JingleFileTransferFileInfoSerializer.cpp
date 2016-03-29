/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferFileInfoSerializer.h>

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/StringCodecs/Base64.h>

namespace Swift {

JingleFileTransferFileInfoSerializer::JingleFileTransferFileInfoSerializer() {
}

std::string JingleFileTransferFileInfoSerializer::serializePayload(boost::shared_ptr<JingleFileTransferFileInfo> fileInfo) const {

	XMLElement fileElement("file", "");

	if (fileInfo->getDate() != stringToDateTime("")) {
		fileElement.addNode(boost::make_shared<XMLElement>("date", "", dateTimeToString(fileInfo->getDate())));
	}

	if (!fileInfo->getDescription().empty()) {
		fileElement.addNode(boost::make_shared<XMLElement>("desc", "", fileInfo->getDescription()));
	}

	if (!fileInfo->getMediaType().empty()) {
		fileElement.addNode(boost::make_shared<XMLElement>("media-type", "", fileInfo->getMediaType()));
	}

	if (!fileInfo->getName().empty()) {
		fileElement.addNode(boost::make_shared<XMLElement>("name", "", fileInfo->getName()));
	}

	if (fileInfo->getSupportsRangeRequests()) {
		boost::shared_ptr<XMLElement> range = boost::make_shared<XMLElement>("range");
		if (fileInfo->getRangeOffset() != 0) {
			range->setAttribute("offset", boost::lexical_cast<std::string>(fileInfo->getRangeOffset()));
		}
		fileElement.addNode(range);
	}

	if (fileInfo->getSize() > 0) {
		fileElement.addNode(boost::make_shared<XMLElement>("size", "", boost::lexical_cast<std::string>(fileInfo->getSize())));
	}

	foreach (JingleFileTransferFileInfo::HashElementMap::value_type hashElement, fileInfo->getHashes()) {
		boost::shared_ptr<XMLElement> hash = boost::make_shared<XMLElement>("hash", "urn:xmpp:hashes:1", Base64::encode(hashElement.second));
		hash->setAttribute("algo", hashElement.first);
		fileElement.addNode(hash);
	}

	return fileElement.serialize();
}

}
