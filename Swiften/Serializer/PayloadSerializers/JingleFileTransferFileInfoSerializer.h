/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/JingleFileTransferFileInfo.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
    class SWIFTEN_API JingleFileTransferFileInfoSerializer : public GenericPayloadSerializer<JingleFileTransferFileInfo> {
        public:
            JingleFileTransferFileInfoSerializer();

            virtual std::string serializePayload(std::shared_ptr<JingleFileTransferFileInfo>)  const;
    };
}
