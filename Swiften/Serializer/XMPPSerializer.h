/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StreamType.h>
#include <Swiften/Elements/ToplevelElement.h>
#include <Swiften/Serializer/ElementSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;
    class ProtocolHeader;

    class SWIFTEN_API XMPPSerializer {
        public:
            XMPPSerializer(PayloadSerializerCollection*, StreamType type, bool setExplictNSonTopLevelElements);

            std::string serializeHeader(const ProtocolHeader&) const;
            SafeByteArray serializeElement(std::shared_ptr<ToplevelElement> stanza) const;
            std::string serializeFooter() const;

        private:
            std::string getDefaultNamespace() const;

        private:
            StreamType type_;
            std::vector< std::shared_ptr<ElementSerializer> > serializers_;
    };
}
