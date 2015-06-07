/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/PayloadSerializer.h>

namespace Swift {
	template<typename PAYLOAD_TYPE>
	class SWIFTEN_API GenericPayloadSerializer : public PayloadSerializer {
		public:
			virtual std::string serialize(boost::shared_ptr<Payload> element)  const {
				return serializePayload(boost::dynamic_pointer_cast<PAYLOAD_TYPE>(element));
			}

			virtual bool canSerialize(boost::shared_ptr<Payload> element) const {
				return !!boost::dynamic_pointer_cast<PAYLOAD_TYPE>(element);
			}

			virtual std::string serializePayload(boost::shared_ptr<PAYLOAD_TYPE>) const = 0;
	};
}
