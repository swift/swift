#include "Swiften/Serializer/PayloadSerializers/MUCPayloadSerializer.h"

namespace Swift {

MUCPayloadSerializer::MUCPayloadSerializer() : GenericPayloadSerializer<MUCPayload>() {
}

String MUCPayloadSerializer::serializePayload(boost::shared_ptr<MUCPayload>)  const {
	String result("<x xmlns='http://jabber.org/protocol/muc'/>");
	return result;
}

}
