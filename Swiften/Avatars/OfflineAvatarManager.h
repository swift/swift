/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Avatars/AvatarProvider.h>
#include <Swiften/Base/API.h>

namespace Swift {
    class AvatarStorage;

    class SWIFTEN_API OfflineAvatarManager : public AvatarProvider {
        public:
            OfflineAvatarManager(AvatarStorage*);
            ~OfflineAvatarManager();

            virtual boost::optional<std::string> getAvatarHash(const JID&) const;
            void setAvatar(const JID&, const std::string& hash);

        private:
            AvatarStorage* avatarStorage;
    };
}
