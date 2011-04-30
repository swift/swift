/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
