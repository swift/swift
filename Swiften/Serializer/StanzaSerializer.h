/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Stanza.h>
#include <Swiften/Serializer/ElementSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;
    class XMLElement;

    class SWIFTEN_API StanzaSerializer : public ElementSerializer {
        public:
            StanzaSerializer(const std::string& tag, PayloadSerializerCollection* payloadSerializers, const boost::optional<std::string>& explicitNS = boost::optional<std::string>());

            virtual SafeByteArray serialize(std::shared_ptr<ToplevelElement> element) const;
            virtual SafeByteArray serialize(std::shared_ptr<ToplevelElement> element, const std::string& xmlns) const;
            virtual void setStanzaSpecificAttributes(std::shared_ptr<ToplevelElement>, XMLElement&) const = 0;

        private:
            std::string tag_;
            PayloadSerializerCollection* payloadSerializers_;
            boost::optional<std::string> explicitDefaultNS_;
    };
}
