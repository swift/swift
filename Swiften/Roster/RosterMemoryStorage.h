/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Roster/RosterStorage.h>

namespace Swift {
    class SWIFTEN_API RosterMemoryStorage : public RosterStorage {
        public:
            RosterMemoryStorage();

            virtual std::shared_ptr<RosterPayload> getRoster() const {
                return roster;
            }

            virtual void setRoster(std::shared_ptr<RosterPayload>);

        private:
            std::shared_ptr<RosterPayload> roster;
    };
}
