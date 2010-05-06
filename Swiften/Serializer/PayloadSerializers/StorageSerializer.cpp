/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Serializer/PayloadSerializers/StorageSerializer.h"

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/foreach.h"
#include "Swiften/Serializer/XML/XMLElement.h"
#include "Swiften/Serializer/XML/XMLTextNode.h"

namespace Swift {

StorageSerializer::StorageSerializer() : GenericPayloadSerializer<Storage>() {
}

String StorageSerializer::serializePayload(boost::shared_ptr<Storage> storage)	const {
	XMLElement storageElement("storage", "storage:bookmarks");

	foreach(const Storage::Conference& conference, storage->getConferences()) {
		boost::shared_ptr<XMLElement> conferenceElement(new XMLElement("conference"));
		conferenceElement->setAttribute("name", conference.name);
		conferenceElement->setAttribute("jid", conference.jid);
		conferenceElement->setAttribute("autojoin", conference.autoJoin ? "1" : "0");
		if (!conference.nick.isEmpty()) {
			boost::shared_ptr<XMLElement> nickElement(new XMLElement("nick"));
			nickElement->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode(conference.nick)));
			conferenceElement->addNode(nickElement);
		}
		if (!conference.password.isEmpty()) {
			boost::shared_ptr<XMLElement> passwordElement(new XMLElement("password"));
			passwordElement->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode(conference.password)));
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
