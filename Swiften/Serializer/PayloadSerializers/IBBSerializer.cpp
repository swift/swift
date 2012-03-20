/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/IBBSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/StringCodecs/Base64.h>

namespace Swift {

IBBSerializer::IBBSerializer() {
}

std::string IBBSerializer::serializePayload(boost::shared_ptr<IBB> ibb) const {
	switch(ibb->getAction()) {
		case IBB::Data: {
			XMLElement ibbElement("data", "http://jabber.org/protocol/ibb");
			ibbElement.setAttribute("sid", ibb->getStreamID());
			if (ibb->getSequenceNumber() >= 0) {
				ibbElement.setAttribute("seq", boost::lexical_cast<std::string>(ibb->getSequenceNumber()));
			}
			ibbElement.addNode(boost::make_shared<XMLTextNode>(Base64::encode(ibb->getData())));
			return ibbElement.serialize();
		}
		case IBB::Open: {
			XMLElement ibbElement("open", "http://jabber.org/protocol/ibb");
			ibbElement.setAttribute("sid", ibb->getStreamID());
			switch (ibb->getStanzaType()) {
				case IBB::IQStanza: ibbElement.setAttribute("stanza", "iq"); break;
				case IBB::MessageStanza: ibbElement.setAttribute("stanza", "message"); break;
			}
			assert(ibb->getBlockSize() > 0);
			ibbElement.setAttribute("block-size", boost::lexical_cast<std::string>(ibb->getBlockSize()));
			return ibbElement.serialize();
		}
		case IBB::Close: {
			XMLElement ibbElement("close", "http://jabber.org/protocol/ibb");
			ibbElement.setAttribute("sid", ibb->getStreamID());
			return ibbElement.serialize();
		}
	}
	return "";
}

}
