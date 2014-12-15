/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/Payload.h>
#include <string>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

namespace Swift {
	class PayloadsSerializer {
		public:
			std::string serialize(boost::shared_ptr<Payload> payload);

		private:
			FullPayloadSerializerCollection serializers;
	};
}
