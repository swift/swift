#ifndef SWIFTEN_MessageSerializer_H
#define SWIFTEN_MessageSerializer_H

#include "Swiften/Serializer/GenericStanzaSerializer.h"
#include "Swiften/Elements/Message.h"

namespace Swift {
	class XMLElement;

	class MessageSerializer : public GenericStanzaSerializer<Message> {
		public:
			MessageSerializer(PayloadSerializerCollection* payloadSerializers);

		private:
			void setStanzaSpecificAttributesGeneric(
					boost::shared_ptr<Message> message, 
					XMLElement& element) const;
	};
}

#endif
