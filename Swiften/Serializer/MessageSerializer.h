/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericStanzaSerializer.h>
#include <Swiften/Elements/Message.h>

#include <boost/optional.hpp>

namespace Swift {
	class XMLElement;

	class SWIFTEN_API MessageSerializer : public GenericStanzaSerializer<Message> {
		public:
			MessageSerializer(PayloadSerializerCollection* payloadSerializers, const boost::optional<std::string>& explitNS = boost::optional<std::string>());

		private:
			void setStanzaSpecificAttributesGeneric(
					boost::shared_ptr<Message> message, 
					XMLElement& element) const;
	};
}
