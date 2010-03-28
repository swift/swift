#ifndef SWIFTEN_AuthFailureSerializer_H
#define SWIFTEN_AuthFailureSerializer_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/AuthFailure.h"
#include "Swiften/Serializer/GenericElementSerializer.h"
#include "Swiften/Serializer/XML/XMLElement.h"

namespace Swift {
	class AuthFailureSerializer : public GenericElementSerializer<AuthFailure> {
		public:
			AuthFailureSerializer() : GenericElementSerializer<AuthFailure>() {
			}

			virtual String serialize(boost::shared_ptr<Element>) const {
				return XMLElement("failure", "urn:ietf:params:xml:ns:xmpp-sasl").serialize();
			}
	};
}

#endif
