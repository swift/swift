/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Serializer/GenericStanzaSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
    class SWIFTEN_API IQSerializer : public GenericStanzaSerializer<IQ> {
        public:
            IQSerializer(PayloadSerializerCollection* payloadSerializers, const boost::optional<std::string>& explicitNS = boost::optional<std::string>()) :
                    GenericStanzaSerializer<IQ>("iq", payloadSerializers, explicitNS) {
            }

        private:
            virtual void setStanzaSpecificAttributesGeneric(
                    std::shared_ptr<IQ> iq,
                    XMLElement& element) const {
                switch (iq->getType()) {
                    case IQ::Get: element.setAttribute("type","get"); break;
                    case IQ::Set: element.setAttribute("type","set"); break;
                    case IQ::Result: element.setAttribute("type","result"); break;
                    case IQ::Error: element.setAttribute("type","error"); break;
                }
            }
    };
}
