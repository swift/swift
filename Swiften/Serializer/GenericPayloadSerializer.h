/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_GenericPayloadSerializer_H
#define SWIFTEN_GenericPayloadSerializer_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Serializer/PayloadSerializer.h"
#include "Swiften/Elements/Body.h"

namespace Swift {
	template<typename PAYLOAD_TYPE>
	class GenericPayloadSerializer : public PayloadSerializer {
		public:
			virtual String serialize(boost::shared_ptr<Payload> element)  const {
				return serializePayload(boost::dynamic_pointer_cast<PAYLOAD_TYPE>(element));
			}

			virtual bool canSerialize(boost::shared_ptr<Payload> element) const {
				return dynamic_cast<PAYLOAD_TYPE*>(element.get());
			}

			virtual String serializePayload(boost::shared_ptr<PAYLOAD_TYPE>) const = 0;
	};
}

#endif
