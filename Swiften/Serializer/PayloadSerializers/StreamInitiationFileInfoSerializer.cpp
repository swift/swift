/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/StreamInitiationFileInfoSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Base/DateTime.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>



namespace Swift {

StreamInitiationFileInfoSerializer::StreamInitiationFileInfoSerializer() {
}
								    
std::string StreamInitiationFileInfoSerializer::serializePayload(boost::shared_ptr<StreamInitiationFileInfo> fileInfo) const {
	XMLElement fileElement("file", "http://jabber.org/protocol/si/profile/file-transfer");
	
	if (fileInfo->getDate() != stringToDateTime("")) {
		fileElement.setAttribute("date", dateTimeToString(fileInfo->getDate()));
	}
	fileElement.setAttribute("hash", fileInfo->getHash());
	if (fileInfo->getAlgo() != "md5") {
		fileElement.setAttribute("algo", fileInfo->getAlgo());
	}
	if (!fileInfo->getName().empty()) {
		fileElement.setAttribute("name", fileInfo->getName());
	}
	if (fileInfo->getSize() != 0) {
		fileElement.setAttribute("size", boost::lexical_cast<std::string>(fileInfo->getSize()));
	}
	if (!fileInfo->getDescription().empty()) {
		boost::shared_ptr<XMLElement> desc = boost::make_shared<XMLElement>("desc", "", fileInfo->getDescription());
		fileElement.addNode(desc);
	}
	if (fileInfo->getSupportsRangeRequests()) {
		boost::shared_ptr<XMLElement> range = boost::make_shared<XMLElement>("range");
		if (fileInfo->getRangeOffset() != 0) {
			range->setAttribute("offset", boost::lexical_cast<std::string>(fileInfo->getRangeOffset()));
		}
		fileElement.addNode(range);
	}
	return fileElement.serialize();
}

}
