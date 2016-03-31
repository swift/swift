/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StreamType.h>
#include <Swiften/Elements/ToplevelElement.h>
#include <Swiften/Serializer/ElementSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;
    class CompressRequestSerializer;
    class ProtocolHeader;

    class SWIFTEN_API XMPPSerializer {
        public:
            XMPPSerializer(PayloadSerializerCollection*, StreamType type, bool setExplictNSonTopLevelElements);

            std::string serializeHeader(const ProtocolHeader&) const;
            SafeByteArray serializeElement(boost::shared_ptr<ToplevelElement> stanza) const;
            std::string serializeFooter() const;

        private:
            std::string getDefaultNamespace() const;

        private:
            StreamType type_;
            std::vector< boost::shared_ptr<ElementSerializer> > serializers_;
    };
}
