/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericStanzaSerializer.h>
#include <Swiften/Elements/Message.h>

#include <boost/optional.hpp>

namespace Swift {
	class XMLElement;

	class MessageSerializer : public GenericStanzaSerializer<Message> {
		public:
			MessageSerializer(PayloadSerializerCollection* payloadSerializers, const boost::optional<std::string>& explitNS = boost::optional<std::string>());

		private:
			void setStanzaSpecificAttributesGeneric(
					boost::shared_ptr<Message> message, 
					XMLElement& element) const;
	};
}
