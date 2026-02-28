/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>

#include <Swiften/Avatars/AvatarProvider.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class MUCRegistry;
    class AvatarStorage;
    class StanzaChannel;
    class VCardManager;
    class CryptoProvider;

    class SWIFTEN_API VCardUpdateAvatarManager : public AvatarProvider, public boost::signals2::trackable {
        public:
            VCardUpdateAvatarManager(VCardManager*, StanzaChannel*, AvatarStorage*, CryptoProvider* crypto, MUCRegistry* = nullptr);

            boost::optional<std::string> getAvatarHash(const JID&) const;

        private:
            void handlePresenceReceived(std::shared_ptr<Presence>);
            void handleStanzaChannelAvailableChanged(bool);
            void handleVCardChanged(const JID& from, VCard::ref);
            void setAvatarHash(const JID& from, const std::string& hash);
            JID getAvatarJID(const JID& o) const;

        private:
            VCardManager* vcardManager_;
            AvatarStorage* avatarStorage_;
            CryptoProvider* crypto_;
            MUCRegistry* mucRegistry_;
            std::map<JID, std::string> avatarHashes_;
    };
}
