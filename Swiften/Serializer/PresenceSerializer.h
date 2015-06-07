/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericStanzaSerializer.h>
#include <Swiften/Elements/Presence.h>

#include <boost/optional.hpp>

namespace Swift {
	class SWIFTEN_API PresenceSerializer : public GenericStanzaSerializer<Presence> {
		public:
			PresenceSerializer(PayloadSerializerCollection* payloadSerializers, const boost::optional<std::string>& explicitNS = boost::optional<std::string>());
			~PresenceSerializer() {}

		private:
			virtual void setStanzaSpecificAttributesGeneric(
					boost::shared_ptr<Presence> presence, 
					XMLElement& element) const;
	};
}
