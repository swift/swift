#include "Swiften/Serializer/PayloadSerializers/VCardSerializer.h"

#include <boost/shared_ptr.hpp>

#include "Swiften/Serializer/XML/XMLElement.h"

namespace Swift {

VCardSerializer::VCardSerializer() : GenericPayloadSerializer<VCard>() {
}

String VCardSerializer::serializePayload(boost::shared_ptr<VCard>)  const {
	XMLElement queryElement("vCard", "vcard-temp");
	// TODO
	return queryElement.serialize();
}

}
