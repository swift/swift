#ifndef SWIFTEN_BodySerializer_H
#define SWIFTEN_BodySerializer_H

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Serializer/XML/XMLTextNode.h"
#include "Swiften/Elements/Body.h"

namespace Swift {
	class BodySerializer : public GenericPayloadSerializer<Body> {
		public:
			BodySerializer() : GenericPayloadSerializer<Body>() {}

			virtual String serializePayload(boost::shared_ptr<Body> body)  const {
				XMLTextNode textNode(body->getText());
				return "<body>" + textNode.serialize() + "</body>";
			}
	};
}

#endif
