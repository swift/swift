/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */


#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/JingleContentPayload.h>

namespace Swift {
	class PayloadSerializerCollection;
	
	class JingleContentPayloadSerializer : public GenericPayloadSerializer<JingleContentPayload> {
		public:
			JingleContentPayloadSerializer();
			
			virtual std::string serializePayload(boost::shared_ptr<JingleContentPayload>)  const;
			
		private:
			std::string creatorToString(JingleContentPayload::Creator creator) const;
	};
}
