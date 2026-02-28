/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Disco/DummyEntityCapsProvider.h>

namespace Swift {

DiscoInfo::ref DummyEntityCapsProvider::getCaps(const JID& jid) const {
    std::map<JID, DiscoInfo::ref>::const_iterator i = caps.find(jid);
    if (i != caps.end()) {
        return i->second;
    }
    return DiscoInfo::ref();
}

DiscoInfo::ref DummyEntityCapsProvider::getCapsCached(const JID& jid) {
    return getCaps(jid);
}

}
