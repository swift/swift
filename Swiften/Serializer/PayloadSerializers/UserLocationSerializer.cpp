/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/UserLocationSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Base/DateTime.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {

UserLocationSerializer::UserLocationSerializer() {
}

std::string UserLocationSerializer::serializePayload(
		boost::shared_ptr<UserLocation> payload) const {
	XMLElement result("geoloc", "http://jabber.org/protocol/geoloc");
	if (boost::optional<std::string> value = payload->getArea()) {
		result.addNode(boost::make_shared<XMLElement>("area", "", *value));
	}
	if (boost::optional<float> value = payload->getAltitude()) {
		result.addNode(boost::make_shared<XMLElement>("alt", "", boost::lexical_cast<std::string>(*value)));
	}
	if (boost::optional<std::string> value = payload->getLocality()) {
		result.addNode(boost::make_shared<XMLElement>("locality", "", *value));
	}
	if (boost::optional<float> value = payload->getLatitude()) {
		result.addNode(boost::make_shared<XMLElement>("lat", "", boost::lexical_cast<std::string>(*value)));
	}
	if (boost::optional<float> value = payload->getAccuracy()) {
		result.addNode(boost::make_shared<XMLElement>("accuracy", "", boost::lexical_cast<std::string>(*value)));
	}
	if (boost::optional<std::string> value = payload->getDescription()) {
		result.addNode(boost::make_shared<XMLElement>("description", "", *value));
	}
	if (boost::optional<std::string> value = payload->getCountryCode()) {
		result.addNode(boost::make_shared<XMLElement>("countrycode", "", *value));
	}
	if (boost::optional<boost::posix_time::ptime> value = payload->getTimestamp()) {
		result.addNode(boost::make_shared<XMLElement>("timestamp", "", dateTimeToString(*value)));
	}
	if (boost::optional<std::string> value = payload->getFloor()) {
		result.addNode(boost::make_shared<XMLElement>("floor", "", *value));
	}
	if (boost::optional<std::string> value = payload->getBuilding()) {
		result.addNode(boost::make_shared<XMLElement>("building", "", *value));
	}
	if (boost::optional<std::string> value = payload->getRoom()) {
		result.addNode(boost::make_shared<XMLElement>("room", "", *value));
	}
	if (boost::optional<std::string> value = payload->getCountry()) {
		result.addNode(boost::make_shared<XMLElement>("country", "", *value));
	}
	if (boost::optional<std::string> value = payload->getRegion()) {
		result.addNode(boost::make_shared<XMLElement>("region", "", *value));
	}
	if (boost::optional<std::string> value = payload->getURI()) {
		result.addNode(boost::make_shared<XMLElement>("uri", "", *value));
	}
	if (boost::optional<float> value = payload->getLongitude()) {
		result.addNode(boost::make_shared<XMLElement>("lon", "", boost::lexical_cast<std::string>(*value)));
	}
	if (boost::optional<float> value = payload->getError()) {
		result.addNode(boost::make_shared<XMLElement>("error", "", boost::lexical_cast<std::string>(*value)));
	}
	if (boost::optional<std::string> value = payload->getPostalCode()) {
		result.addNode(boost::make_shared<XMLElement>("postalcode", "", *value));
	}
	if (boost::optional<float> value = payload->getBearing()) {
		result.addNode(boost::make_shared<XMLElement>("bearing", "", boost::lexical_cast<std::string>(*value)));
	}
	if (boost::optional<std::string> value = payload->getText()) {
		result.addNode(boost::make_shared<XMLElement>("text", "", *value));
	}
	if (boost::optional<std::string> value = payload->getDatum()) {
		result.addNode(boost::make_shared<XMLElement>("datum", "", *value));
	}
	if (boost::optional<std::string> value = payload->getStreet()) {
		result.addNode(boost::make_shared<XMLElement>("street", "", *value));
	}
	if (boost::optional<float> value = payload->getSpeed()) {
		result.addNode(boost::make_shared<XMLElement>("speed", "", boost::lexical_cast<std::string>(*value)));
	}
	return result.serialize();
}

}
