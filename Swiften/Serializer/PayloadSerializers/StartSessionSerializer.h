#ifndef SWIFTEN_StartSessionSerializer_H
#define SWIFTEN_StartSessionSerializer_H

#include <boost/lexical_cast.hpp>

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/StartSession.h"

namespace Swift {
	class StartSessionSerializer : public GenericPayloadSerializer<StartSession> {
		public:
			StartSessionSerializer() : GenericPayloadSerializer<StartSession>() {}

			virtual String serializePayload(boost::shared_ptr<StartSession>)  const {
				return XMLElement("session", "urn:ietf:params:xml:ns:xmpp-session").serialize();
			}
	};
}

#endif
