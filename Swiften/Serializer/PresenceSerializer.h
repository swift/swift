/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericStanzaSerializer.h>
#include <Swiften/Elements/Presence.h>

namespace Swift {
	class PresenceSerializer : public GenericStanzaSerializer<Presence> {
		public:
			PresenceSerializer(PayloadSerializerCollection* payloadSerializers);

		private:
			virtual void setStanzaSpecificAttributesGeneric(
					boost::shared_ptr<Presence> presence, 
					XMLElement& element) const;
	};
}
