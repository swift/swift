/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Queries/IQChannel.h>
#include <Swiften/TLS/Certificate.h>

namespace Swift {
    class SWIFTEN_API StanzaChannel : public IQChannel {
        public:
            virtual void sendMessage(std::shared_ptr<Message>) = 0;
            virtual void sendPresence(std::shared_ptr<Presence>) = 0;
            virtual bool isAvailable() const = 0;
            virtual bool getStreamManagementEnabled() const = 0;
            virtual std::vector<Certificate::ref> getPeerCertificateChain() const = 0;

            boost::signals2::signal<void (bool /* isAvailable */)> onAvailableChanged;
            boost::signals2::signal<void (std::shared_ptr<Message>)> onMessageReceived;
            boost::signals2::signal<void (std::shared_ptr<Presence>) > onPresenceReceived;
            boost::signals2::signal<void (std::shared_ptr<Stanza>)> onStanzaAcked;
    };
}
