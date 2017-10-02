/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/signals2.hpp>

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/XMPPEvents/StanzaEvent.h>

#include <SwifTools/Notifier/Notifier.h>

namespace Swift {
    class EventController;
    class Notifier;
    class AvatarManager;
    class NickResolver;
    class JID;
    class UIEventStream;
    class SettingsProvider;

    class EventNotifier {
        public:
            EventNotifier(EventController* eventController, Notifier* notifier, AvatarManager* avatarManager, NickResolver* nickResolver);
            ~EventNotifier();

            boost::signals2::signal<void (const JID&)> onNotificationActivated;

        private:
            void handleEventAdded(std::shared_ptr<StanzaEvent>);
            void handleNotificationActivated(JID jid);

        private:
            EventController* eventController;
            Notifier* notifier;
            AvatarManager* avatarManager;
            NickResolver* nickResolver;
    };
}
