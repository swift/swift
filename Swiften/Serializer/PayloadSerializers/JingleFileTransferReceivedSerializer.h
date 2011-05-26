/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */


#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/JingleFileTransferReceived.h>

namespace Swift {
	class PayloadSerializerCollection;
	class XMLElement;
	
	class JingleFileTransferReceivedSerializer : public GenericPayloadSerializer<JingleFileTransferReceived> {
		public:
			JingleFileTransferReceivedSerializer();
			
			virtual std::string serializePayload(boost::shared_ptr<JingleFileTransferReceived>)  const;
	};
}
