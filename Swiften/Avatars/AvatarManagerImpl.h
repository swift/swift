/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Avatars/AvatarManager.h>
#include <Swiften/Avatars/CombinedAvatarProvider.h>

namespace Swift {
    class MUCRegistry;
    class AvatarStorage;
    class StanzaChannel;
    class VCardManager;
    class VCardUpdateAvatarManager;
    class VCardAvatarManager;
    class OfflineAvatarManager;
    class CryptoProvider;

    class AvatarManagerImpl : public AvatarManager {
        public:
            AvatarManagerImpl(VCardManager*, StanzaChannel*, AvatarStorage*, CryptoProvider* crypto, MUCRegistry* = nullptr);
            virtual ~AvatarManagerImpl();

            virtual boost::filesystem::path getAvatarPath(const JID&) const;
            virtual ByteArray getAvatar(const JID&) const;

        private:
            void handleCombinedAvatarChanged(const JID& jid);


        private:
            CombinedAvatarProvider combinedAvatarProvider;
            AvatarStorage* avatarStorage;
            VCardUpdateAvatarManager* vcardUpdateAvatarManager;
            VCardAvatarManager* vcardAvatarManager;
            OfflineAvatarManager* offlineAvatarManager;
    };
}
