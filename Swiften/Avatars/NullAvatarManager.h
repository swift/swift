/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Avatars/AvatarManager.h>

namespace Swift {
    class NullAvatarManager : public AvatarManager {
        public:
            virtual boost::filesystem::path getAvatarPath(const JID&) const {
                return boost::filesystem::path();
            }

            virtual ByteArray getAvatar(const JID&) const {
                return ByteArray();
            }
    };
}
