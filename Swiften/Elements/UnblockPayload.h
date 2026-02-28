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
    class SWIFTEN_API UnblockPayload : public Payload {
        public:
            UnblockPayload(const std::vector<JID>& jids = std::vector<JID>()) : items(jids) {
            }

            void addItem(const JID& item) {
                items.push_back(item);
            }

            const std::vector<JID>& getItems() const {
                return items;
            }

        private:
            std::vector<JID> items;
    };
}
