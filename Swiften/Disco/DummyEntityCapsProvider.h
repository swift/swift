/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>

#include <Swiften/Base/API.h>
#include <Swiften/Disco/EntityCapsProvider.h>

namespace Swift {
    class SWIFTEN_API DummyEntityCapsProvider : public EntityCapsProvider {
        public:
            DummyEntityCapsProvider() {
            }

            DiscoInfo::ref getCaps(const JID& jid) const;

            std::map<JID, DiscoInfo::ref> caps;
    };
}
