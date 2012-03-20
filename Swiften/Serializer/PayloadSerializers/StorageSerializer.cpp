/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/StorageSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

StorageSerializer::StorageSerializer() : GenericPayloadSerializer<Storage>() {
}

std::string StorageSerializer::serializePayload(boost::shared_ptr<Storage> storage)	const {
	XMLElement storageElement("storage", "storage:bookmarks");

	foreach(const Storage::Room& room, storage->getRooms()) {
		boost::shared_ptr<XMLElement> conferenceElement(new XMLElement("conference"));
		conferenceElement->setAttribute("name", room.name);
		conferenceElement->setAttribute("jid", room.jid);
		conferenceElement->setAttribute("autojoin", room.autoJoin ? "1" : "0");
		if (!room.nick.empty()) {
			boost::shared_ptr<XMLElement> nickElement(new XMLElement("nick"));
			nickElement->addNode(boost::make_shared<XMLTextNode>(room.nick));
			conferenceElement->addNode(nickElement);
		}
		if (room.password) {
			boost::shared_ptr<XMLElement> passwordElement(new XMLElement("password"));
			passwordElement->addNode(boost::make_shared<XMLTextNode>(*room.password));
			conferenceElement->addNode(passwordElement);
		}
		storageElement.addNode(conferenceElement);
	}

	foreach(const Storage::URL& url, storage->getURLs()) {
		boost::shared_ptr<XMLElement> urlElement(new XMLElement("url"));
		urlElement->setAttribute("name", url.name);
		urlElement->setAttribute("url", url.url);
		storageElement.addNode(urlElement);
	}

	return storageElement.serialize();
}

}
