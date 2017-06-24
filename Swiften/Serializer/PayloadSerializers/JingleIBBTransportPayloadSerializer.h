/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API JingleIBBTransportPayloadSerializer : public GenericPayloadSerializer<JingleIBBTransportPayload> {
        public:
            JingleIBBTransportPayloadSerializer();

            virtual std::string serializePayload(std::shared_ptr<JingleIBBTransportPayload>)  const;
    };
}
