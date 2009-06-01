#ifndef SWIFTEN_AuthSuccessSerializer_H
#define SWIFTEN_AuthSuccessSerializer_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/AuthSuccess.h"
#include "Swiften/Serializer/GenericElementSerializer.h"
#include "Swiften/Serializer/XML/XMLElement.h"

namespace Swift {
	class AuthSuccessSerializer : public GenericElementSerializer<AuthSuccess> {
		public:
			AuthSuccessSerializer() : GenericElementSerializer<AuthSuccess>() {
			}

			virtual String serialize(boost::shared_ptr<Element>) const {
				return XMLElement("success", "urn:ietf:params:xml:ns:xmpp-sasl").serialize();
			}
	};
}

#endif
