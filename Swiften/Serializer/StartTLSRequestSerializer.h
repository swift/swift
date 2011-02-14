/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_StartTLSRequestSerializer_H
#define SWIFTEN_StartTLSRequestSerializer_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/StartTLSRequest.h"
#include "Swiften/Serializer/GenericElementSerializer.h"
#include "Swiften/Serializer/XML/XMLElement.h"

namespace Swift {
	class StartTLSRequestSerializer : public GenericElementSerializer<StartTLSRequest> {
		public:
			StartTLSRequestSerializer() : GenericElementSerializer<StartTLSRequest>() {
			}

			virtual std::string serialize(boost::shared_ptr<Element>) const {
				return XMLElement("starttls", "urn:ietf:params:xml:ns:xmpp-tls").serialize();
			}
	};
}

#endif
