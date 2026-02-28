/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <set>

#include <boost/signals2.hpp>

#include <Swiften/Avatars/AvatarManager.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Network/Timer.h>

#include <SwifTools/Notifier/Notifier.h>

namespace Swift {
    class TimerFactory;
    class StanzaChannel;
    class MUCRegistry;
    class NickResolver;
    class PresenceOracle;

    class PresenceNotifier {
        public:
            PresenceNotifier(StanzaChannel* stanzaChannel, Notifier* notifier, const MUCRegistry* mucRegistry, AvatarManager* avatarManager, NickResolver* nickResolver, const PresenceOracle* presenceOracle, TimerFactory* timerFactory);
            ~PresenceNotifier();

            void setInitialQuietPeriodMS(int ms);

            boost::signals2::signal<void (const JID&)> onNotificationActivated;

        private:
            void handlePresenceReceived(std::shared_ptr<Presence>);
            void handleStanzaChannelAvailableChanged(bool);
            void handleNotificationActivated(JID jid);
            void handleTimerTick();
            std::string getStatusType(const JID&) const;
            std::string getStatusMessage(const JID&) const;

        private:
            void showNotification(const JID& jid, Notifier::Type type);

        private:
            StanzaChannel* stanzaChannel;
            Notifier* notifier;
            const MUCRegistry* mucRegistry;
            AvatarManager* avatarManager;
            NickResolver* nickResolver;
            const PresenceOracle* presenceOracle;
            TimerFactory* timerFactory;
            std::shared_ptr<Timer> timer;
            bool justInitialized;
            bool inQuietPeriod;
            std::set<JID> availableUsers;
    };
}

