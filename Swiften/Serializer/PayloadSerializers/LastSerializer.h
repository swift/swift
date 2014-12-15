/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/lexical_cast.hpp>

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/Last.h>

namespace Swift {
	class LastSerializer : public GenericPayloadSerializer<Last> {
		public:
			LastSerializer() : GenericPayloadSerializer<Last>() {}

			virtual std::string serializePayload(boost::shared_ptr<Last> last)  const {
				return "<query xmlns='jabber:iq:last' seconds='" + boost::lexical_cast<std::string>(last->getSeconds()) + "'/>";
			}
	};
}
