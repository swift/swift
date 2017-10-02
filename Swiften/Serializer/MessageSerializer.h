/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Serializer/GenericStanzaSerializer.h>

namespace Swift {
    class XMLElement;

    class SWIFTEN_API MessageSerializer : public GenericStanzaSerializer<Message> {
        public:
            MessageSerializer(PayloadSerializerCollection* payloadSerializers, const boost::optional<std::string>& explitNS = boost::optional<std::string>());

        private:
            void setStanzaSpecificAttributesGeneric(
                    std::shared_ptr<Message> message,
                    XMLElement& element) const;
    };
}
