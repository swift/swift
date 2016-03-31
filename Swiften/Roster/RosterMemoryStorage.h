/*
 * Copyright (c) 2011 Isode Limited.
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

            virtual boost::shared_ptr<RosterPayload> getRoster() const {
                return roster;
            }

            virtual void setRoster(boost::shared_ptr<RosterPayload>);

        private:
            boost::shared_ptr<RosterPayload> roster;
    };
}
