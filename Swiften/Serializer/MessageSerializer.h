/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericStanzaSerializer.h>
#include <Swiften/Elements/Message.h>

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
