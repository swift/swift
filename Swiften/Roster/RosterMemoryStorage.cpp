/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Roster/RosterMemoryStorage.h>

#include <memory>

namespace Swift {

RosterMemoryStorage::RosterMemoryStorage() {
}

void RosterMemoryStorage::setRoster(std::shared_ptr<RosterPayload> r) {
    roster.reset();
    if (r) {
        roster = std::make_shared<RosterPayload>(*r);
    }
}

}
