/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API BlockPayload : public Payload {
        public:
            BlockPayload(const std::vector<JID>& jids = std::vector<JID>()) : items(jids) {
            }

            void addItem(const JID& jid) {
                items.push_back(jid);
            }

            const std::vector<JID>& getItems() const {
                return items;
            }

        private:
            std::vector<JID> items;
    };
}
