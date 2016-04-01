/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Serializer/GenericStanzaSerializer.h>

namespace Swift {
    class SWIFTEN_API PresenceSerializer : public GenericStanzaSerializer<Presence> {
        public:
            PresenceSerializer(PayloadSerializerCollection* payloadSerializers, const boost::optional<std::string>& explicitNS = boost::optional<std::string>());
            ~PresenceSerializer() {}

        private:
            virtual void setStanzaSpecificAttributesGeneric(
                    std::shared_ptr<Presence> presence,
                    XMLElement& element) const;
    };
}
