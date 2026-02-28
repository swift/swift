/*
 * Copyright (c) 2012 Catalin Badea
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <set>

#include <boost/bind.hpp>
#include <boost/signals2.hpp>

#include <Swiften/History/HistoryStorage.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Presence/PresenceOracle.h>

#include <Swift/Controllers/UIEvents/UIEventStream.h>

namespace Swift {
    class HistoryWindowFactory;
    class HistoryWindow;
    class Roster;
    class RosterItem;
    class ContactRosterItem;
    class HistoryController;
    class NickResolver;
    class AvatarManager;

    class HistoryViewController {
        public:
            HistoryViewController(const JID& selfJID, UIEventStream* uiEventStream, HistoryController* historyController, NickResolver* nickResolver, AvatarManager* avatarManager, PresenceOracle* presenceOracle, HistoryWindowFactory* historyWindowFactory);
            ~HistoryViewController();

        private:
            void handleUIEvent(std::shared_ptr<UIEvent> event);
            void handleSelectedContactChanged(RosterItem* item);
            void handleNewMessage(const HistoryMessage& message);
            void handleReturnPressed(const std::string& keyword);
            void handleScrollReachedTop(const boost::gregorian::date& date);
            void handleScrollReachedBottom(const boost::gregorian::date& date);
            void handlePreviousButtonClicked();
            void handleNextButtonClicked();
            void handleCalendarClicked(const boost::gregorian::date& date);
            void handlePresenceChanged(Presence::ref presence);
            void handleAvatarChanged(const JID& jid);

            void addNewMessage(const HistoryMessage& message, bool addAtTheTop);
            void reset();
            Presence::ref getPresence(const JID& jid, bool isMUC);

        private:
            JID selfJID_;
            UIEventStream* uiEventStream_;
            HistoryController* historyController_;
            NickResolver* nickResolver_;
            AvatarManager* avatarManager_;
            PresenceOracle* presenceOracle_;
            HistoryWindowFactory* historyWindowFactory_;
            HistoryWindow* historyWindow_;
            Roster* roster_;

            std::map<HistoryMessage::Type, ContactsMap> contacts_;
            ContactRosterItem* selectedItem_;
            HistoryMessage::Type selectedItemType_ = HistoryMessage::Chat;
            boost::gregorian::date currentResultDate_;
    };
}
