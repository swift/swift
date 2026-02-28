/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StatusShow.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API StatusShowSerializer : public GenericPayloadSerializer<StatusShow> {
        public:
            StatusShowSerializer() : GenericPayloadSerializer<StatusShow>() {}

            virtual std::string serializePayload(std::shared_ptr<StatusShow> statusShow)  const {
                if (statusShow->getType () == StatusShow::Online || statusShow->getType() == StatusShow::None) {
                    return "";
                }
                else {
                    std::string result("<show>");
                    switch (statusShow->getType()) {
                        case StatusShow::Away: result += "away"; break;
                        case StatusShow::XA: result += "xa"; break;
                        case StatusShow::FFC: result += "chat"; break;
                        case StatusShow::DND: result += "dnd"; break;
                        case StatusShow::Online: assert(false); break;
                        case StatusShow::None: assert(false); break;
                    }
                    result += "</show>";
                    return result;
                }
            }
    };
}
