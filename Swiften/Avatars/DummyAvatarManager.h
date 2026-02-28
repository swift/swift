/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>

#include <Swiften/Avatars/AvatarManager.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class DummyAvatarManager : public AvatarManager {
        public:
            virtual boost::filesystem::path getAvatarPath(const JID& j) const {
                return boost::filesystem::path("/avatars") / j.toString();
            }

            virtual ByteArray getAvatar(const JID& jid) const {
                std::map<JID, ByteArray>::const_iterator i = avatars.find(jid);
                if (i != avatars.end()) {
                    return i->second;
                }
                else {
                    return ByteArray();
                }
            }

            std::map<JID, ByteArray> avatars;
    };
}
