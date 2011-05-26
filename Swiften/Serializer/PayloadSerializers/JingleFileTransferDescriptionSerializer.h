/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */


#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/JingleFileTransferDescription.h>



namespace Swift {
	class PayloadSerializerCollection;
	class XMLElement;
	
	class JingleFileTransferDescriptionSerializer : public GenericPayloadSerializer<JingleFileTransferDescription> {
		public:
			JingleFileTransferDescriptionSerializer();
			
			virtual std::string serializePayload(boost::shared_ptr<JingleFileTransferDescription>)  const;
	};
}
