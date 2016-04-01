/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class XMPPRoster;
    class MUCRegistry;
    class VCardManager;

    class SWIFTEN_API NickResolver {
        public:
            NickResolver(const JID& ownJID, XMPPRoster* xmppRoster, VCardManager* vcardManager, MUCRegistry* mucRegistry);

            std::string jidToNick(const JID& jid);

            boost::signal<void (const JID&, const std::string& /*previousNick*/)> onNickChanged;

        private:
            void handleVCardReceived(const JID& jid, VCard::ref vCard);
            void handleJIDUpdated(const JID& jid, const std::string& previousNick, const std::vector<std::string>& groups);
            void handleJIDAdded(const JID& jid);

        private:
            JID ownJID_;
            std::string ownNick_;
            XMPPRoster* xmppRoster_;
            MUCRegistry* mucRegistry_;
            VCardManager* vcardManager_;
    };
}
