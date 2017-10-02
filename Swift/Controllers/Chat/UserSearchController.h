/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <boost/signals2.hpp>

#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/DiscoItems.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/SearchPayload.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/JID/JID.h>

#include <Swift/Controllers/Contact.h>

namespace Swift {
    class UIEventStream;
    class UIEvent;
    class UserSearchWindow;
    class UserSearchWindowFactory;
    class IQRouter;
    class DiscoServiceWalker;
    class RosterController;
    class VCardManager;
    class ContactSuggester;
    class AvatarManager;
    class PresenceOracle;
    class ProfileSettingsProvider;

    class UserSearchResult {
        public:
            UserSearchResult(const JID& jid, const std::map<std::string, std::string>& fields) : jid_(jid), fields_(fields) {}
            const JID& getJID() const {return jid_;}
            const std::map<std::string, std::string>& getFields() const {return fields_;}
        private:
            JID jid_;
            std::map<std::string, std::string> fields_;
    };

    class UserSearchController {
        public:
            enum class Type {AddContact, StartChat, InviteToChat};
            UserSearchController(Type type, const JID& jid, UIEventStream* uiEventStream, VCardManager* vcardManager, UserSearchWindowFactory* userSearchWindowFactory, IQRouter* iqRouter, RosterController* rosterController, ContactSuggester* contactSuggester, AvatarManager* avatarManager, PresenceOracle* presenceOracle, ProfileSettingsProvider* settings);
            ~UserSearchController();

            UserSearchWindow* getUserSearchWindow();
            void setCanInitiateImpromptuMUC(bool supportsImpromptu);

        private:
            void handleUIEvent(std::shared_ptr<UIEvent> event);
            void handleFormRequested(const JID& service);
            void handleDiscoServiceFound(const JID& jid, std::shared_ptr<DiscoInfo> info);
            void handleDiscoWalkFinished();
            void handleFormResponse(std::shared_ptr<SearchPayload> items, ErrorPayload::ref error);
            void handleSearch(std::shared_ptr<SearchPayload> fields, const JID& jid);
            void handleSearchResponse(std::shared_ptr<SearchPayload> results, ErrorPayload::ref error);
            void handleNameSuggestionRequest(const JID& jid);
            void handleContactSuggestionsRequested(std::string text);
            void handleVCardChanged(const JID& jid, VCard::ref vcard);
            void handleAvatarChanged(const JID& jid);
            void handlePresenceChanged(Presence::ref presence);
            void handleJIDUpdateRequested(const std::vector<JID>& jids);
            void handleJIDAddRequested(const std::vector<JID>& jids);
            void handleJIDEditingFinished(const JID& jid);
            Contact::ref convertJIDtoContact(const JID& jid);
            void endDiscoWalker();
            void initializeUserWindow();

            void loadSavedDirectories();
            void addToSavedDirectories(const JID& jid);

        private:
            Type type_;
            JID jid_;
            JID suggestionsJID_;
            UIEventStream* uiEventStream_;
            VCardManager* vcardManager_;
            UserSearchWindowFactory* factory_;
            IQRouter* iqRouter_;
            RosterController* rosterController_;
            UserSearchWindow* window_;
            DiscoServiceWalker* discoWalker_;
            ContactSuggester* contactSuggester_;
            AvatarManager* avatarManager_;
            PresenceOracle* presenceOracle_;
            std::vector<JID> savedDirectories_;
            ProfileSettingsProvider* settings_;
    };
}
