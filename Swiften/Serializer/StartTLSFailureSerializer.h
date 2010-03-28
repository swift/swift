#ifndef SWIFTEN_StartTLSFailureSerializer_H
#define SWIFTEN_StartTLSFailureSerializer_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/StartTLSFailure.h"
#include "Swiften/Serializer/GenericElementSerializer.h"
#include "Swiften/Serializer/XML/XMLElement.h"

namespace Swift {
	class StartTLSFailureSerializer : public GenericElementSerializer<StartTLSFailure> {
		public:
			StartTLSFailureSerializer() : GenericElementSerializer<StartTLSFailure>() {
			}

			virtual String serialize(boost::shared_ptr<Element>) const {
				return XMLElement("failure", "urn:ietf:params:xml:ns:xmpp-tls").serialize();
			}
	};
}

#endif
