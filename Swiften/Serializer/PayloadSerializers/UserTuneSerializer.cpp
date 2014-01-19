/*
 * Copyright (c) 2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/UserTuneSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {

UserTuneSerializer::UserTuneSerializer() {
}

std::string UserTuneSerializer::serializePayload(
		boost::shared_ptr<UserTune> payload) const {
	XMLElement result("tune", "http://jabber.org/protocol/tune");
	if (boost::optional<std::string> value = payload->getArtist()) {
		result.addNode(boost::make_shared<XMLElement>("artist", "", *value));
	}
	if (boost::optional<unsigned int> value = payload->getLength()) {
		result.addNode(boost::make_shared<XMLElement>("length", "", boost::lexical_cast<std::string>(*value)));
	}
	if (boost::optional<unsigned int> value = payload->getRating()) {
		result.addNode(boost::make_shared<XMLElement>("rating", "", boost::lexical_cast<std::string>(*value)));
	}
	if (boost::optional<std::string> value = payload->getSource()) {
		result.addNode(boost::make_shared<XMLElement>("source", "", *value));
	}
	if (boost::optional<std::string> value = payload->getTitle()) {
		result.addNode(boost::make_shared<XMLElement>("title", "", *value));
	}
	if (boost::optional<std::string> value = payload->getTrack()) {
		result.addNode(boost::make_shared<XMLElement>("track", "", *value));
	}
	if (boost::optional<std::string> value = payload->getURI()) {
		result.addNode(boost::make_shared<XMLElement>("uri", "", *value));
	}
	return result.serialize();
}

}
