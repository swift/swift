/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/Elements/Thread.h>

namespace Swift {
	class SWIFTEN_API ThreadSerializer : public GenericPayloadSerializer<Thread> {
		public:
			ThreadSerializer();
			virtual ~ThreadSerializer();

			virtual std::string serializePayload(boost::shared_ptr<Thread> thread) const;
	};
}
