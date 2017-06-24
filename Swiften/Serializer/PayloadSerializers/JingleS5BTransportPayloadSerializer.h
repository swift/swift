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
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API JingleS5BTransportPayloadSerializer : public GenericPayloadSerializer<JingleS5BTransportPayload> {
        public:
            JingleS5BTransportPayloadSerializer();

            virtual std::string serializePayload(std::shared_ptr<JingleS5BTransportPayload>)  const;

        private:
            std::string modeToString(JingleS5BTransportPayload::Mode) const;
            std::string typeToString(JingleS5BTransportPayload::Candidate::Type) const;
    };
}
