/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/JingleContentPayload.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API JingleContentPayloadSerializer : public GenericPayloadSerializer<JingleContentPayload> {
        public:
            JingleContentPayloadSerializer();

            virtual std::string serializePayload(boost::shared_ptr<JingleContentPayload>)  const;

        private:
            std::string creatorToString(JingleContentPayload::Creator creator) const;
    };
}
