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
#include <Swiften/Elements/JinglePayload.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API JinglePayloadSerializer : public GenericPayloadSerializer<JinglePayload> {
        public:
            JinglePayloadSerializer(PayloadSerializerCollection*);

            virtual std::string serializePayload(std::shared_ptr<JinglePayload>)  const;

        private:
            std::string actionToString(JinglePayload::Action action) const;
            std::string reasonTypeToString(JinglePayload::Reason::Type type) const;

        private:
            PayloadSerializerCollection* serializers;
    };
}
