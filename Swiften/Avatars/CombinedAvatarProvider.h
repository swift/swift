/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <vector>

#include <Swiften/Avatars/AvatarProvider.h>
#include <Swiften/Base/API.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API CombinedAvatarProvider : public AvatarProvider {
        public:
            virtual boost::optional<std::string> getAvatarHash(const JID&) const;

            void addProvider(AvatarProvider*);
            void removeProvider(AvatarProvider*);

        private:
            void handleAvatarChanged(const JID&);
            boost::optional<std::string> getCombinedAvatarAndCache(const JID&) const;

        private:
            std::vector<AvatarProvider*> providers;
            mutable std::map<JID, std::string> avatars;
    };
}
