#include "Swiften/Serializer/PayloadSerializers/UnitTest/PayloadsSerializer.h"

#include <cppunit/extensions/HelperMacros.h>

#include "Swiften/Serializer/PayloadSerializer.h"

namespace Swift {

String PayloadsSerializer::serialize(boost::shared_ptr<Payload> payload) {
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
