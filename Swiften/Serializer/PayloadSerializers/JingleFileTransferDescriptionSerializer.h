/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/JingleFileTransferDescription.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API JingleFileTransferDescriptionSerializer : public GenericPayloadSerializer<JingleFileTransferDescription> {
        public:
            JingleFileTransferDescriptionSerializer();

            virtual std::string serializePayload(std::shared_ptr<JingleFileTransferDescription>)  const;
    };
}
