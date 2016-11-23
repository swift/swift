/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Elements/RosterPayload.h>

namespace Swift {

boost::optional<RosterItemPayload> RosterPayload::getItem(const JID& jid) const {
    for (const auto& item : items_) {
        // FIXME: MSVC rejects this. Find out why.
        //if (item.getJID() == jid) {
        if (item.getJID().equals(jid, JID::WithResource)) {
            return boost::optional<RosterItemPayload>(item);
        }
    }
    return boost::optional<RosterItemPayload>();
}

}
