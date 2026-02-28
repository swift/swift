/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Chat/UserSearchController.h>

#include <memory>

#include <boost/bind.hpp>

#include <Swiften/Avatars/AvatarManager.h>
#include <Swiften/Base/String.h>
#include <Swiften/Disco/DiscoServiceWalker.h>
#include <Swiften/Disco/GetDiscoInfoRequest.h>
#include <Swiften/Disco/GetDiscoItemsRequest.h>
#include <Swiften/Presence/PresenceOracle.h>
#include <Swiften/VCards/VCardManager.h>

#include <Swift/Controllers/ContactEditController.h>
#include <Swift/Controllers/ContactSuggester.h>
#include <Swift/Controllers/Intl.h>
#include <Swift/Controllers/ProfileSettingsProvider.h>
#include <Swift/Controllers/Roster/RosterController.h>
#include <Swift/Controllers/UIEvents/RequestAddUserDialogUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestChatWithUserDialogUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestInviteToMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIInterfaces/UserSearchWindow.h>
#include <Swift/Controllers/UIInterfaces/UserSearchWindowFactory.h>

namespace Swift {

static const std::string SEARCHED_DIRECTORIES = "searchedDirectories";

UserSearchController::UserSearchController(Type type, const JID& jid, UIEventStream* uiEventStream, VCardManager* vcardManager, UserSearchWindowFactory* factory, IQRouter* iqRouter, RosterController* rosterController, ContactSuggester* contactSuggester, AvatarManager* avatarManager, PresenceOracle* presenceOracle, ProfileSettingsProvider* settings) : type_(type), jid_(jid), uiEventStream_(uiEventStream), vcardManager_(vcardManager), factory_(factory), iqRouter_(iqRouter), rosterController_(rosterController), contactSuggester_(contactSuggester), avatarManager_(avatarManager), presenceOracle_(presenceOracle), settings_(settings) {
    uiEventStream_->onUIEvent.connect(boost::bind(&UserSearchController::handleUIEvent, this, _1));
    vcardManager_->onVCardChanged.connect(boost::bind(&UserSearchController::handleVCardChanged, this, _1, _2));
    avatarManager_->onAvatarChanged.connect(boost::bind(&UserSearchController::handleAvatarChanged, this, _1));
    presenceOracle_->onPresenceChange.connect(boost::bind(&UserSearchController::handlePresenceChanged, this, _1));
    window_ = nullptr;
    discoWalker_ = nullptr;
    loadSavedDirectories();
}

UserSearchController::~UserSearchController() {
    endDiscoWalker();
    delete discoWalker_;
    if (window_) {
        window_->onNameSuggestionRequested.disconnect(boost::bind(&UserSearchController::handleNameSuggestionRequest, this, _1));
        window_->onFormRequested.disconnect(boost::bind(&UserSearchController::handleFormRequested, this, _1));
        window_->onSearchRequested.disconnect(boost::bind(&UserSearchController::handleSearch, this, _1, _2));
        window_->onContactSuggestionsRequested.disconnect(boost::bind(&UserSearchController::handleContactSuggestionsRequested, this, _1));
        window_->onJIDUpdateRequested.disconnect(boost::bind(&UserSearchController::handleJIDUpdateRequested, this, _1));
        window_->onJIDAddRequested.disconnect(boost::bind(&UserSearchController::handleJIDAddRequested, this, _1));
        window_->onJIDEditFieldChanged.disconnect(boost::bind(&UserSearchController::handleJIDEditingFinished, this, _1));
        delete window_;
    }
    presenceOracle_->onPresenceChange.disconnect(boost::bind(&UserSearchController::handlePresenceChanged, this, _1));
    avatarManager_->onAvatarChanged.disconnect(boost::bind(&UserSearchController::handleAvatarChanged, this, _1));
    vcardManager_->onVCardChanged.disconnect(boost::bind(&UserSearchController::handleVCardChanged, this, _1, _2));
    uiEventStream_->onUIEvent.disconnect(boost::bind(&UserSearchController::handleUIEvent, this, _1));
}

UserSearchWindow* UserSearchController::getUserSearchWindow() {
    initializeUserWindow();
    assert(window_);
    return window_;
}

void UserSearchController::setCanInitiateImpromptuMUC(bool supportsImpromptu) {
    if (!window_) {
        initializeUserWindow();
    }
    if (window_) {
        window_->setCanStartImpromptuChats(supportsImpromptu);
    } // Else doesn't support search
}

void UserSearchController::handleUIEvent(std::shared_ptr<UIEvent> event) {
    bool handle = false;
    std::shared_ptr<RequestAddUserDialogUIEvent> addUserRequest = std::shared_ptr<RequestAddUserDialogUIEvent>();
    auto inviteToMUCRequest = RequestInviteToMUCUIEvent::ref();
    switch (type_) {
        case Type::AddContact:
            if ((addUserRequest = std::dynamic_pointer_cast<RequestAddUserDialogUIEvent>(event))) {
                handle = true;
            }
            break;
        case Type::StartChat:
            if (std::dynamic_pointer_cast<RequestChatWithUserDialogUIEvent>(event)) {
                handle = true;
            }
            break;
        case Type::InviteToChat:
            if ((inviteToMUCRequest = std::dynamic_pointer_cast<RequestInviteToMUCUIEvent>(event))) {
                handle = true;
            }
            break;
    }
    if (handle) {
        initializeUserWindow();
        window_->show();
        window_->addSavedServices(savedDirectories_);
        if (addUserRequest) {
            const std::string& name = addUserRequest->getPredefinedName();
            const JID& jid = addUserRequest->getPredefinedJID();
            if (!name.empty() && jid.isValid()) {
                window_->prepopulateJIDAndName(jid, name);
            }
        }
        else if (inviteToMUCRequest) {
            window_->setCanSupplyDescription(!inviteToMUCRequest->isImpromptu());
            window_->setJIDs(inviteToMUCRequest->getInvites());
            window_->setOriginator(inviteToMUCRequest->getOriginator());
        }
        return;
    }
}

void UserSearchController::handleFormRequested(const JID& service) {
    window_->setSearchError(false);
    window_->setServerSupportsSearch(true);

    //Abort a previous search if is active
    endDiscoWalker();
    delete discoWalker_;
    discoWalker_ = new DiscoServiceWalker(service, iqRouter_);
    discoWalker_->onServiceFound.connect(boost::bind(&UserSearchController::handleDiscoServiceFound, this, _1, _2));
    discoWalker_->onWalkComplete.connect(boost::bind(&UserSearchController::handleDiscoWalkFinished, this));
    discoWalker_->beginWalk();
}

void UserSearchController::endDiscoWalker() {
    if (discoWalker_) {
        discoWalker_->endWalk();
        discoWalker_->onServiceFound.disconnect(boost::bind(&UserSearchController::handleDiscoServiceFound, this, _1, _2));
        discoWalker_->onWalkComplete.disconnect(boost::bind(&UserSearchController::handleDiscoWalkFinished, this));
    }
}

void UserSearchController::handleDiscoServiceFound(const JID& jid, std::shared_ptr<DiscoInfo> info) {
    //bool isUserDirectory = false;
    bool supports55 = false;
    // TODO: Cleanup code
    for (const auto& identity : info->getIdentities()) {
        if ((identity.getCategory() == "directory"
            && identity.getType() == "user")) {
            //isUserDirectory = true;
        }
    }
    std::vector<std::string> features = info->getFeatures();
    supports55 = std::find(features.begin(), features.end(), DiscoInfo::JabberSearchFeature) != features.end();
    if (/*isUserDirectory && */supports55) { //FIXME: once M-Link correctly advertises directoryness.
        /* Abort further searches.*/
        endDiscoWalker();
        std::shared_ptr<GenericRequest<SearchPayload> > searchRequest(new GenericRequest<SearchPayload>(IQ::Get, jid, std::make_shared<SearchPayload>(), iqRouter_));
        searchRequest->onResponse.connect(boost::bind(&UserSearchController::handleFormResponse, this, _1, _2));
        searchRequest->send();
    }
}

void UserSearchController::handleFormResponse(std::shared_ptr<SearchPayload> fields, ErrorPayload::ref error) {
    if (error || !fields) {
        window_->setServerSupportsSearch(false);
        return;
    }
    window_->setSearchFields(fields);
}

void UserSearchController::handleSearch(std::shared_ptr<SearchPayload> fields, const JID& jid) {
    addToSavedDirectories(jid);
    std::shared_ptr<GenericRequest<SearchPayload> > searchRequest(new GenericRequest<SearchPayload>(IQ::Set, jid, fields, iqRouter_));
    searchRequest->onResponse.connect(boost::bind(&UserSearchController::handleSearchResponse, this, _1, _2));
    searchRequest->send();
}

void UserSearchController::handleSearchResponse(std::shared_ptr<SearchPayload> resultsPayload, ErrorPayload::ref error) {
    if (error || !resultsPayload) {
        window_->setSearchError(true);
        return;
    }

    std::vector<UserSearchResult> results;

    if (resultsPayload->getForm()) {
        window_->setResultsForm(resultsPayload->getForm());
    } else {
        for (auto&& item : resultsPayload->getItems()) {
            JID jid(item.jid);
            std::map<std::string, std::string> fields;
            fields["first"] = item.first;
            fields["last"] = item.last;
            fields["nick"] = item.nick;
            fields["email"] = item.email;
            UserSearchResult result(jid, fields);
            results.push_back(result);
        }
        window_->setResults(results);
    }
}

void UserSearchController::handleNameSuggestionRequest(const JID &jid) {
    suggestionsJID_= jid;
    VCard::ref vcard = vcardManager_->getVCardAndRequestWhenNeeded(jid);
    if (vcard) {
        handleVCardChanged(jid, vcard);
    }
}

void UserSearchController::handleJIDEditingFinished(const JID& jid) {
    if (jid.isValid()) {
        if (rosterController_->getItem(jid)) {
            window_->setWarning(QT_TRANSLATE_NOOP("", "This contact is already on your contact list."));
        }
        else if (jid.getNode().empty()) {
            window_->setWarning(QT_TRANSLATE_NOOP("", "Part of the address you have entered is missing. An address has a structure of 'user@example.com'."));
        }
        else {
            window_->setWarning(boost::optional<std::string>());
        }
    }
    else {
        window_->setWarning(QT_TRANSLATE_NOOP("", "The address you have entered is invalid."));
    }
}

void UserSearchController::handleContactSuggestionsRequested(std::string text) {
    const std::vector<JID> existingJIDs = window_->getJIDs();
    std::vector<Contact::ref> suggestions = contactSuggester_->getSuggestions(text, false);
    /* do not suggest contacts that have already been added to the chat list */
    std::vector<Contact::ref>::iterator i = suggestions.begin();
    while (i != suggestions.end()) {
        bool found = false;
        for (const auto& jid : existingJIDs) {
            if ((*i)->jid == jid) {
                found = true;
                break;
            }
        }

        // remove contact suggestions which are already on the contact list in add-contact-mode
        if (type_ == Type::AddContact) {
            if (!found && !!rosterController_->getItem((*i)->jid)) {
                found = true;
            }
        }

        if (found) {
            i = suggestions.erase(i);
        } else {
            i++;
        }
    }
    window_->setContactSuggestions(suggestions);
}

void UserSearchController::handleVCardChanged(const JID& jid, VCard::ref vcard) {
    if (jid == suggestionsJID_) {
        window_->setNameSuggestions(ContactEditController::nameSuggestionsFromVCard(vcard));
        suggestionsJID_ = JID();
    }
    handleJIDUpdateRequested(std::vector<JID>(1, jid));
}

void UserSearchController::handleAvatarChanged(const JID& jid) {
    handleJIDUpdateRequested(std::vector<JID>(1, jid));
}

void UserSearchController::handlePresenceChanged(Presence::ref presence) {
    handleJIDUpdateRequested(std::vector<JID>(1, presence->getFrom().toBare()));
}

void UserSearchController::handleJIDUpdateRequested(const std::vector<JID>& jids) {
    if (window_) {
        std::vector<Contact::ref> updates;
        for (const auto& jid : jids) {
            updates.push_back(convertJIDtoContact(jid));
        }
        window_->updateContacts(updates);
    }
}

void UserSearchController::handleJIDAddRequested(const std::vector<JID>& jids) {
    std::vector<Contact::ref> contacts;
    for (const auto& jid : jids) {
        contacts.push_back(convertJIDtoContact(jid));
    }
    window_->addContacts(contacts);
}

Contact::ref UserSearchController::convertJIDtoContact(const JID& jid) {
    Contact::ref contact = std::make_shared<Contact>();
    contact->jid = jid;

    // name lookup
    boost::optional<XMPPRosterItem> rosterItem = rosterController_->getItem(jid);
    if (rosterItem && !rosterItem->getName().empty()) {
        contact->name = rosterItem->getName();
    } else {
        VCard::ref vcard = vcardManager_->getVCard(jid);
        if (vcard && !vcard->getFullName().empty()) {
            contact->name = vcard->getFullName();
        } else {
            contact->name = jid.toString();
        }
    }

    // presence lookup
    Presence::ref presence = presenceOracle_->getAccountPresence(jid);
    if (presence) {
        contact->statusType = presence->getShow();
    } else {
        contact->statusType = StatusShow::None;
    }

    // avatar lookup
    contact->avatarPath = avatarManager_->getAvatarPath(jid);
    return contact;
}

void UserSearchController::handleDiscoWalkFinished() {
    window_->setServerSupportsSearch(false);
    endDiscoWalker();
}

void UserSearchController::initializeUserWindow() {
    if (!window_) {
        auto windowType = UserSearchWindow::Type::AddContact;
        switch(type_) {
            case Type::AddContact:
                windowType = UserSearchWindow::Type::AddContact;
                break;
            case Type::StartChat:
                windowType = UserSearchWindow::Type::ChatToContact;
                break;
            case Type::InviteToChat:
                windowType = UserSearchWindow::Type::InviteToChat;
                break;
        }

        window_ = factory_->createUserSearchWindow(windowType, uiEventStream_, rosterController_->getGroups());
        if (!window_) {
            // UI Doesn't support user search
            return;
        }
        window_->onNameSuggestionRequested.connect(boost::bind(&UserSearchController::handleNameSuggestionRequest, this, _1));
        window_->onFormRequested.connect(boost::bind(&UserSearchController::handleFormRequested, this, _1));
        window_->onSearchRequested.connect(boost::bind(&UserSearchController::handleSearch, this, _1, _2));
        window_->onContactSuggestionsRequested.connect(boost::bind(&UserSearchController::handleContactSuggestionsRequested, this, _1));
        window_->onJIDUpdateRequested.connect(boost::bind(&UserSearchController::handleJIDUpdateRequested, this, _1));
        window_->onJIDAddRequested.connect(boost::bind(&UserSearchController::handleJIDAddRequested, this, _1));
        window_->onJIDEditFieldChanged.connect(boost::bind(&UserSearchController::handleJIDEditingFinished, this, _1));
        window_->setSelectedService(JID(jid_.getDomain()));
        window_->clear();
    }
}

void UserSearchController::loadSavedDirectories() {
    savedDirectories_.clear();
    for (auto&& stringItem : String::split(settings_->getStringSetting(SEARCHED_DIRECTORIES), '\n')) {
        if(!stringItem.empty()) {
            savedDirectories_.push_back(JID(stringItem));
        }
    }
}

void UserSearchController::addToSavedDirectories(const JID& jid) {
    if (!jid.isValid()) {
        return;
    }

    savedDirectories_.erase(std::remove(savedDirectories_.begin(), savedDirectories_.end(), jid), savedDirectories_.end());
    savedDirectories_.insert(savedDirectories_.begin(), jid);

    std::string collapsed;
    int i = 0;
    for (const auto& jidItem : savedDirectories_) {
        if (i >= 15) {
            break;
        }
        if (!collapsed.empty()) {
            collapsed += "\n";
        }
        collapsed += jidItem.toString();
        ++i;
    }
    settings_->storeString(SEARCHED_DIRECTORIES, collapsed);
    window_->addSavedServices(savedDirectories_);
}

}
