/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/StanzaSerializer.h>

namespace Swift {
    template<typename STANZA_TYPE>
    class SWIFTEN_API GenericStanzaSerializer : public StanzaSerializer {
        public:
            GenericStanzaSerializer(const std::string& tag, PayloadSerializerCollection* payloadSerializers, const boost::optional<std::string>& explicitNS = boost::optional<std::string>()) : StanzaSerializer(tag, payloadSerializers, explicitNS) {}

            virtual bool canSerialize(std::shared_ptr<ToplevelElement> element) const {
                return dynamic_cast<STANZA_TYPE*>(element.get()) != nullptr;
            }

            virtual void setStanzaSpecificAttributes(
                    std::shared_ptr<ToplevelElement> stanza,
                    XMLElement& element) const {
                setStanzaSpecificAttributesGeneric(
                        std::dynamic_pointer_cast<STANZA_TYPE>(stanza), element);
            }

            virtual void setStanzaSpecificAttributesGeneric(
                    std::shared_ptr<STANZA_TYPE>,
                    XMLElement&) const = 0;
    };
}
