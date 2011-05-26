/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */


#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/JinglePayload.h>

namespace Swift {
	class PayloadSerializerCollection;
	class XMLElement;
	
	class JinglePayloadSerializer : public GenericPayloadSerializer<JinglePayload> {
		public:
			JinglePayloadSerializer(PayloadSerializerCollection*);
			
			virtual std::string serializePayload(boost::shared_ptr<JinglePayload>)  const;
			
		private:
			std::string actionToString(JinglePayload::Action action) const;
			std::string reasonTypeToString(JinglePayload::Reason::Type type) const;
		
		private:
			PayloadSerializerCollection* serializers;
	};
}
