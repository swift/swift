/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */


#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>

namespace Swift {
	class PayloadSerializerCollection;
	class XMLElement;
	
	class JingleS5BTransportPayloadSerializer : public GenericPayloadSerializer<JingleS5BTransportPayload> {
		public:
			JingleS5BTransportPayloadSerializer();
			
			virtual std::string serializePayload(boost::shared_ptr<JingleS5BTransportPayload>)  const;
			
		private:
			std::string modeToString(JingleS5BTransportPayload::Mode) const;
			std::string typeToString(JingleS5BTransportPayload::Candidate::Type) const;
	};
}
