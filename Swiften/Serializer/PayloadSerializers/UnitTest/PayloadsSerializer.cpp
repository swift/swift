/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/UnitTest/PayloadsSerializer.h>

#include <cppunit/extensions/HelperMacros.h>

#include <Swiften/Serializer/PayloadSerializer.h>

namespace Swift {

std::string PayloadsSerializer::serialize(boost::shared_ptr<Payload> payload) {
	PayloadSerializer* serializer = serializers.getPayloadSerializer(payload);
	if (serializer) {
		return serializer->serialize(payload);
	}
	else {
		CPPUNIT_ASSERT(false);
		return "";
	}
}


}
