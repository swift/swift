/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/AccountController.h>

#include <cstdlib>
#include <memory>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/String.h>
#include <Swiften/Base/format.h>
#include <Swiften/Client/Client.h>
#include <Swiften/Client/ClientBlockListManager.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/Client/NickResolver.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Client/Storages.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Disco/CapsInfoGenerator.h>
#include <Swiften/Disco/ClientDiscoManager.h>
#include <Swiften/Disco/GetDiscoInfoRequest.h>
#include <Swiften/Elements/ChatState.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/VCardUpdate.h>
#include <Swiften/FileTransfer/FileTransferManager.h>
#include <Swiften/Network/NetworkFactories.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Presence/PresenceSender.h>
#include <Swiften/Queries/Requests/EnableCarbonsRequest.h>
#include <Swiften/StringCodecs/Base64.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/VCards/GetVCardRequest.h>
#include <Swiften/VCards/VCardManager.h>

#ifdef SWIFTEN_PLATFORM_WIN32
#include <Swiften/SASL/WindowsAuthentication.h>
#endif

#include <Swift/Controllers/AdHocManager.h>
#include <Swift/Controllers/BlockListController.h>
#include <Swift/Controllers/BuildVersion.h>
#include <Swift/Controllers/Chat/Chattables.h>
#include <Swift/Controllers/Chat/ChatsManager.h>
#include <Swift/Controllers/Chat/MUCController.h>
#include <Swift/Controllers/Chat/UserSearchController.h>
#include <Swift/Controllers/ContactEditController.h>
#include <Swift/Controllers/ContactSuggester.h>
#include <Swift/Controllers/ContactsFromXMPPRoster.h>
#include <Swift/Controllers/EventNotifier.h>
#include <Swift/Controllers/EventWindowController.h>
#include <Swift/Controllers/FdpFormSubmitController.h>
#include <Swift/Controllers/FileTransfer/FileTransferOverview.h>
#include <Swift/Controllers/FileTransferListController.h>
#include <Swift/Controllers/Highlighting/HighlightEditorController.h>
#include <Swift/Controllers/Highlighting/HighlightManager.h>
#include <Swift/Controllers/HistoryController.h>
#include <Swift/Controllers/HistoryViewController.h>
#include <Swift/Controllers/Intl.h>
#include <Swift/Controllers/PresenceNotifier.h>
#include <Swift/Controllers/ProfileController.h>
#include <Swift/Controllers/Roster/RosterController.h>
#include <Swift/Controllers/SettingConstants.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swift/Controllers/ShowProfileController.h>
#include <Swift/Controllers/SoundEventController.h>
#include <Swift/Controllers/SoundPlayer.h>
#include <Swift/Controllers/StatusTracker.h>
#include <Swift/Controllers/Storages/CertificateStorageFactory.h>
#include <Swift/Controllers/Storages/CertificateStorageTrustChecker.h>
#include <Swift/Controllers/Storages/StoragesFactory.h>
#include <Swift/Controllers/SystemTray.h>
#include <Swift/Controllers/SystemTrayController.h>
#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestChatUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIInterfaces/LoginWindow.h>
#include <Swift/Controllers/UIInterfaces/LoginWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/MainWindow.h>
#include <Swift/Controllers/UIInterfaces/UIFactory.h>
#include <Swift/Controllers/WhiteboardManager.h>
#include <Swift/Controllers/XMLConsoleController.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>
#include <Swift/Controllers/XMPPURIController.h>

#include <SwifTools/Dock/Dock.h>
#include <SwifTools/Idle/IdleDetector.h>
#include <SwifTools/Notifier/TogglableNotifier.h>

namespace Swift {

static const std::string CLIENT_NAME = "Swift";
static const std::string CLIENT_NODE = "http://swift.im";


AccountController::AccountController(
        EventLoop* eventLoop,
        NetworkFactories* networkFactories,
        UIFactory* uiFactories,
        SettingsProvider* settings,
        SystemTray* systemTray,
        SoundPlayer* soundPlayer,
        StoragesFactory* storagesFactory,
        CertificateStorageFactory* certificateStorageFactory,
        Dock* dock,
        Notifier* notifier,
        URIHandler* uriHandler,
        IdleDetector* idleDetector,
        const std::map<std::string, std::string>& emoticons,
        bool useDelayForLatency) :
            eventLoop_(eventLoop),
            networkFactories_(networkFactories),
            uiFactory_(uiFactories),
            storagesFactory_(storagesFactory),
            certificateStorageFactory_(certificateStorageFactory),
            settings_(settings),
            uriHandler_(uriHandler),
            idleDetector_(idleDetector),
            loginWindow_(nullptr) ,
            useDelayForLatency_(useDelayForLatency),
            ftOverview_(nullptr),
            emoticons_(emoticons),
            fdpFormSubmitController_(nullptr) {
    storages_ = nullptr;
    certificateStorage_ = nullptr;
    certificateTrustChecker_ = nullptr;
    statusTracker_ = nullptr;
    presenceNotifier_ = nullptr;
    eventNotifier_ = nullptr;
    rosterController_ = nullptr;
    chatsManager_ = nullptr;
    historyController_ = nullptr;
    historyViewController_ = nullptr;
    eventWindowController_ = nullptr;
    profileController_ = nullptr;
    blockListController_ = nullptr;
    showProfileController_ = nullptr;
    contactEditController_ = nullptr;
    userSearchControllerChat_ = nullptr;
    userSearchControllerAdd_ = nullptr;
    userSearchControllerInvite_ = nullptr;
    contactsFromRosterProvider_ = nullptr;
    contactSuggesterWithoutRoster_ = nullptr;
    contactSuggesterWithRoster_ = nullptr;
    whiteboardManager_ = nullptr;
    adHocManager_ = nullptr;
    quitRequested_ = false;
    clientInitialized_ = false;
    offlineRequested_ = false;

    timeBeforeNextReconnect_ = -1;
    dock_ = dock;
    uiEventStream_ = new UIEventStream();

    notifier_ = new TogglableNotifier(notifier);
    notifier_->setPersistentEnabled(settings_->getSetting(SettingConstants::SHOW_NOTIFICATIONS));
    eventController_ = new EventController();
    eventController_->onEventQueueLengthChange.connect(boost::bind(&AccountController::handleEventQueueLengthChange, this, _1));

    systemTrayController_ = new SystemTrayController(eventController_, systemTray);
    loginWindow_ = uiFactory_->createLoginWindow(uiEventStream_);
    loginWindow_->setShowNotificationToggle(!notifier->isExternallyConfigured());

    highlightManager_ = new HighlightManager(settings_);
    highlightEditorController_ = new HighlightEditorController(uiEventStream_, uiFactory_, highlightManager_);

    soundEventController_ = new SoundEventController(eventController_, soundPlayer, settings, highlightManager_);

    xmppURIController_ = new XMPPURIController(uriHandler_, uiEventStream_);

    loginWindow_->onLoginRequest.connect(boost::bind(&AccountController::handleLoginRequest, this, _1, _2, _3, _4, _5, _6, _7));
    loginWindow_->onPurgeSavedLoginRequest.connect(boost::bind(&AccountController::handlePurgeSavedLoginRequest, this, _1));
    loginWindow_->onCancelLoginRequest.connect(boost::bind(&AccountController::handleCancelLoginRequest, this));
    loginWindow_->onQuitRequest.connect(boost::bind(&AccountController::handleQuitRequest, this));

    idleDetector_->setIdleTimeSeconds(settings->getSetting(SettingConstants::IDLE_TIMEOUT));
    idleDetector_->onIdleChanged.connect(boost::bind(&AccountController::handleInputIdleChanged, this, _1));

    xmlConsoleController_ = new XMLConsoleController(uiEventStream_, uiFactory_);

    fileTransferListController_ = new FileTransferListController(uiEventStream_, uiFactory_);

    settings_->onSettingChanged.connect(boost::bind(&AccountController::handleSettingChanged, this, _1));

}

AccountController::~AccountController() {
    idleDetector_->onIdleChanged.disconnect(boost::bind(&AccountController::handleInputIdleChanged, this, _1));

    purgeCachedCredentials();
    //setManagersOffline();
    eventController_->disconnectAll();

    resetClient();
    delete highlightEditorController_;
    delete highlightManager_;
    delete fileTransferListController_;
    delete xmlConsoleController_;
    delete xmppURIController_;
    delete soundEventController_;
    delete systemTrayController_;
    delete eventController_;
    delete notifier_;
    delete uiEventStream_;
}

void AccountController::purgeCachedCredentials() {
    safeClear(password_);
}

void AccountController::resetClient() {
    purgeCachedCredentials();
    resetCurrentError();
    resetPendingReconnects();
    vCardPhotoHash_.clear();
    delete contactEditController_;
    contactEditController_ = nullptr;
    delete profileController_;
    profileController_ = nullptr;
    delete showProfileController_;
    showProfileController_ = nullptr;
    delete eventWindowController_;
    eventWindowController_ = nullptr;
    delete chatsManager_;
    chatsManager_ = nullptr;
#ifdef SWIFT_EXPERIMENTAL_HISTORY
    delete historyViewController_;
    historyViewController_ = nullptr;
    delete historyController_;
    historyController_ = nullptr;
#endif
    fileTransferListController_->setFileTransferOverview(nullptr);
    delete ftOverview_;
    ftOverview_ = nullptr;
    delete blockListController_;
    blockListController_ = nullptr;
    delete rosterController_;
    rosterController_ = nullptr;
    chattables_.reset();
    delete eventNotifier_;
    eventNotifier_ = nullptr;
    delete presenceNotifier_;
    presenceNotifier_ = nullptr;
    delete certificateTrustChecker_;
    certificateTrustChecker_ = nullptr;
    delete certificateStorage_;
    certificateStorage_ = nullptr;
    delete storages_;
    storages_ = nullptr;
    delete statusTracker_;
    statusTracker_ = nullptr;
    delete profileSettings_;
    profileSettings_ = nullptr;
    delete userSearchControllerChat_;
    userSearchControllerChat_ = nullptr;
    delete userSearchControllerAdd_;
    userSearchControllerAdd_ = nullptr;
    delete userSearchControllerInvite_;
    userSearchControllerInvite_ = nullptr;
    delete contactSuggesterWithoutRoster_;
    contactSuggesterWithoutRoster_ = nullptr;
    delete contactSuggesterWithRoster_;
    contactSuggesterWithRoster_ = nullptr;
    delete contactsFromRosterProvider_;
    contactsFromRosterProvider_ = nullptr;
    delete adHocManager_;
    adHocManager_ = nullptr;
    delete whiteboardManager_;
    whiteboardManager_ = nullptr;
    clientInitialized_ = false;
}

void AccountController::handleSettingChanged(const std::string& settingPath) {
    if (settingPath == SettingConstants::SHOW_NOTIFICATIONS.getKey()) {
        notifier_->setPersistentEnabled(settings_->getSetting(SettingConstants::SHOW_NOTIFICATIONS));
    }
}

void AccountController::resetPendingReconnects() {
    timeBeforeNextReconnect_ = -1;
    if (reconnectTimer_) {
        reconnectTimer_->stop();
        reconnectTimer_.reset();
    }
    resetCurrentError();
}

void AccountController::resetCurrentError() {
    if (lastDisconnectError_) {
        lastDisconnectError_->conclude();
        lastDisconnectError_ = std::shared_ptr<ErrorEvent>();
    }
}

void AccountController::handleConnected() {
    boundJID_ = client_->getJID();
    resetCurrentError();
    resetPendingReconnects();

    if (settings_->getSetting(SettingConstants::FORGET_PASSWORDS)) {
        purgeCachedCredentials();
    }

    bool freshLogin = rosterController_ == nullptr;
    myStatusLooksOnline_ = true;
    if (freshLogin) {
        profileController_ = new ProfileController(client_->getVCardManager(), uiFactory_, uiEventStream_);
        showProfileController_ = new ShowProfileController(client_->getVCardManager(), uiFactory_, uiEventStream_);
        ftOverview_ = new FileTransferOverview(client_->getFileTransferManager());
        fileTransferListController_->setFileTransferOverview(ftOverview_);
        chattables_ = std::make_unique<Chattables>();
        rosterController_ = new RosterController(boundJID_, client_->getRoster(), client_->getAvatarManager(), uiFactory_, client_->getNickManager(), client_->getNickResolver(), client_->getPresenceOracle(), client_->getSubscriptionManager(), eventController_, uiEventStream_, client_->getIQRouter(), settings_, client_->getEntityCapsProvider(), client_->getClientBlockListManager(), client_->getVCardManager(), *chattables_);
        rosterController_->onChangeStatusRequest.connect(boost::bind(&AccountController::handleChangeStatusRequest, this, _1, _2));
        rosterController_->onSignOutRequest.connect(boost::bind(&AccountController::signOut, this));
        rosterController_->getWindow()->onShowCertificateRequest.connect(boost::bind(&AccountController::handleShowCertificateRequest, this));

        blockListController_ = new BlockListController(client_->getClientBlockListManager(), uiEventStream_, uiFactory_, eventController_);

        contactEditController_ = new ContactEditController(rosterController_, client_->getVCardManager(), uiFactory_, uiEventStream_);
        whiteboardManager_ = new WhiteboardManager(uiFactory_, uiEventStream_, client_->getNickResolver(), client_->getWhiteboardSessionManager());

        /* Doing this early as an ordering fix. Various things later will
         * want to have the user's nick available and this means it will
         * be before they receive stanzas that need it (e.g. bookmarks).*/
        client_->getVCardManager()->requestOwnVCard();

        contactSuggesterWithoutRoster_ = new ContactSuggester();
        contactSuggesterWithRoster_ = new ContactSuggester();

        userSearchControllerInvite_ = new UserSearchController(UserSearchController::Type::InviteToChat, jid_, uiEventStream_, client_->getVCardManager(), uiFactory_, client_->getIQRouter(), rosterController_, contactSuggesterWithRoster_, client_->getAvatarManager(), client_->getPresenceOracle(), profileSettings_);
#ifdef SWIFT_EXPERIMENTAL_HISTORY
        historyController_ = new HistoryController(storages_->getHistoryStorage());
        historyViewController_ = new HistoryViewController(jid_, uiEventStream_, historyController_, client_->getNickResolver(), client_->getAvatarManager(), client_->getPresenceOracle(), uiFactory_);
        chatsManager_ = new ChatsManager(jid_, client_->getStanzaChannel(), client_->getIQRouter(), eventController_, uiFactory_, uiFactory_, client_->getNickResolver(), client_->getPresenceOracle(), client_->getPresenceSender(), uiEventStream_, uiFactory_, useDelayForLatency_, networkFactories_->getTimerFactory(), client_->getMUCRegistry(), client_->getEntityCapsProvider(), client_->getMUCManager(), uiFactory_, profileSettings_, ftOverview_, client_->getRoster(), !settings_->getSetting(SettingConstants::REMEMBER_RECENT_CHATS), settings_, historyController_, whiteboardManager_, highlightManager_, client_->getClientBlockListManager(), emoticons_, client_->getVCardManager());
#else
        chatsManager_ = new ChatsManager(jid_, client_->getStanzaChannel(), client_->getIQRouter(), eventController_, uiFactory_, uiFactory_, client_->getNickResolver(), client_->getPresenceOracle(), client_->getPresenceSender(), uiEventStream_, uiFactory_, useDelayForLatency_, networkFactories_->getTimerFactory(), client_->getMUCRegistry(), client_->getEntityCapsProvider(), client_->getMUCManager(), uiFactory_, profileSettings_, ftOverview_, client_->getRoster(), !settings_->getSetting(SettingConstants::REMEMBER_RECENT_CHATS), settings_, nullptr, whiteboardManager_, highlightManager_, client_->getClientBlockListManager(), emoticons_, client_->getVCardManager(), *chattables_);
#endif
        contactsFromRosterProvider_ = new ContactsFromXMPPRoster(client_->getRoster(), client_->getAvatarManager(), client_->getPresenceOracle());
        contactSuggesterWithoutRoster_->addContactProvider(chatsManager_);
        contactSuggesterWithRoster_->addContactProvider(chatsManager_);
        contactSuggesterWithRoster_->addContactProvider(contactsFromRosterProvider_);
        highlightEditorController_->setContactSuggester(contactSuggesterWithoutRoster_);

        client_->onMessageReceived.connect(boost::bind(&ChatsManager::handleIncomingMessage, chatsManager_, _1));
        chatsManager_->setAvatarManager(client_->getAvatarManager());

        eventWindowController_ = new EventWindowController(eventController_, uiFactory_);

        loginWindow_->morphInto(rosterController_->getWindow());

        DiscoInfo discoInfo;
        discoInfo.addIdentity(DiscoInfo::Identity(CLIENT_NAME, "client", "pc"));
        discoInfo.addFeature(DiscoInfo::ChatStatesFeature);
        discoInfo.addFeature(DiscoInfo::SecurityLabelsFeature);
        discoInfo.addFeature(DiscoInfo::MessageCorrectionFeature);
#ifdef SWIFT_EXPERIMENTAL_FT
        discoInfo.addFeature(DiscoInfo::JingleFeature);
        discoInfo.addFeature(DiscoInfo::JingleFTFeature);
        discoInfo.addFeature(DiscoInfo::JingleTransportsIBBFeature);
        discoInfo.addFeature(DiscoInfo::JingleTransportsS5BFeature);
#endif
#ifdef SWIFT_EXPERIMENTAL_WB
        discoInfo.addFeature(DiscoInfo::WhiteboardFeature);
#endif
        discoInfo.addFeature(DiscoInfo::MessageDeliveryReceiptsFeature);
        client_->getDiscoManager()->setCapsNode(CLIENT_NODE);
        client_->getDiscoManager()->setDiscoInfo(discoInfo);

        userSearchControllerChat_ = new UserSearchController(UserSearchController::Type::StartChat, jid_, uiEventStream_, client_->getVCardManager(), uiFactory_, client_->getIQRouter(), rosterController_, contactSuggesterWithRoster_, client_->getAvatarManager(), client_->getPresenceOracle(), profileSettings_);
        userSearchControllerAdd_ = new UserSearchController(UserSearchController::Type::AddContact, jid_, uiEventStream_, client_->getVCardManager(), uiFactory_, client_->getIQRouter(), rosterController_, contactSuggesterWithoutRoster_, client_->getAvatarManager(), client_->getPresenceOracle(), profileSettings_);
        adHocManager_ = new AdHocManager(JID(boundJID_.getDomain()), uiFactory_, client_->getIQRouter(), uiEventStream_, rosterController_->getWindow());

        chatsManager_->onImpromptuMUCServiceDiscovered.connect(boost::bind(&UserSearchController::setCanInitiateImpromptuMUC, userSearchControllerChat_, _1));
    }
    loginWindow_->setIsLoggingIn(false);

    client_->requestRoster();

    GetDiscoInfoRequest::ref discoInfoRequest = GetDiscoInfoRequest::create(JID(boundJID_.getDomain()), client_->getIQRouter());
    discoInfoRequest->onResponse.connect(boost::bind(&AccountController::handleServerDiscoInfoResponse, this, _1, _2));
    discoInfoRequest->send();

    client_->getVCardManager()->requestOwnVCard();

    rosterController_->setJID(boundJID_);
    rosterController_->setEnabled(true);
    rosterController_->getWindow()->setStreamEncryptionStatus(client_->isStreamEncrypted());
    profileController_->setAvailable(true);
    contactEditController_->setAvailable(true);
    /* Send presence later to catch all the incoming presences. */
    sendPresence(statusTracker_->getNextPresence());

    /* Enable chats last of all, so rejoining MUCs has the right sent presence */
    assert(chatsManager_);
    chatsManager_->setOnline(true);
    adHocManager_->setOnline(true);
}

void AccountController::handleEventQueueLengthChange(size_t count) {
    dock_->setNumberOfPendingMessages(count);
}

void AccountController::reconnectAfterError() {
    if (reconnectTimer_) {
        reconnectTimer_->stop();
    }
    performLoginFromCachedCredentials();
}

void AccountController::handleChangeStatusRequest(StatusShow::Type show, const std::string &statusText) {
    std::shared_ptr<Presence> presence(new Presence());
    if (show == StatusShow::None) {
        // Note: this is misleading, None doesn't mean unavailable on the wire.
        presence->setType(Presence::Unavailable);
        resetPendingReconnects();
        myStatusLooksOnline_ = false;
        offlineRequested_ = true;
    }
    else {
        offlineRequested_ = false;
        presence->setShow(show);
    }
    presence->setStatus(statusText);
    statusTracker_->setRequestedPresence(presence);
    if (presence->getType() != Presence::Unavailable) {
        profileSettings_->storeInt("lastShow", presence->getShow());
        profileSettings_->storeString("lastStatus", presence->getStatus());
    }
    if (presence->getType() != Presence::Unavailable && !client_->isAvailable()) {
        performLoginFromCachedCredentials();
    } else {
        sendPresence(presence);
    }
}

void AccountController::sendPresence(std::shared_ptr<Presence> presence) {
    rosterController_->getWindow()->setMyStatusType(presence->getShow());
    rosterController_->getWindow()->setMyStatusText(presence->getStatus());
    systemTrayController_->setMyStatusType(presence->getShow());
    notifier_->setTemporarilyDisabled(presence->getShow() == StatusShow::DND);

    // Add information and send
    presence->updatePayload(std::make_shared<VCardUpdate>(vCardPhotoHash_));
    client_->getPresenceSender()->sendPresence(presence);
    if (presence->getType() == Presence::Unavailable) {
        logout();
    }
}

void AccountController::handleInputIdleChanged(bool idle) {
    if (!statusTracker_) {
        //Haven't logged in yet.
        return;
    }

    if (settings_->getSetting(SettingConstants::IDLE_GOES_OFFLINE)) {
        if (idle) {
            logout();
        }
    }
    else {
        if (idle) {
            if (statusTracker_->goAutoAway(idleDetector_->getIdleTimeSeconds())) {
                if (client_ && client_->isAvailable()) {
                    sendPresence(statusTracker_->getNextPresence());
                }
            }
        } else {
            if (statusTracker_->goAutoUnAway()) {
                if (client_ && client_->isAvailable()) {
                    sendPresence(statusTracker_->getNextPresence());
                }
            }
        }
    }
}

void AccountController::handleShowCertificateRequest() {
    std::vector<Certificate::ref> chain = client_->getStanzaChannel()->getPeerCertificateChain();
    rosterController_->getWindow()->openCertificateDialog(chain);
}

void AccountController::handleLoginRequest(const std::string &username, const std::string &password, const std::string& certificatePath, CertificateWithKey::ref certificate, const ClientOptions& options, bool remember, bool loginAutomatically) {
    jid_ = JID(username);
    if (options.singleSignOn && (!jid_.isValid() || !jid_.getNode().empty())) {
        loginWindow_->setMessage(QT_TRANSLATE_NOOP("", "User address invalid. User address should be of the form 'wonderland.lit'"));
        loginWindow_->setIsLoggingIn(false);
    } else if (!options.singleSignOn && (!jid_.isValid() || jid_.getNode().empty())) {
        loginWindow_->setMessage(QT_TRANSLATE_NOOP("", "User address invalid. User address should be of the form 'alice@wonderland.lit'"));
        loginWindow_->setIsLoggingIn(false);
    } else {
#ifdef SWIFTEN_PLATFORM_WIN32
        if (options.singleSignOn) {
            std::string userName;
            std::string clientName;
            std::string serverName;
            std::shared_ptr<boost::system::error_code> errorCode = getUserNameEx(userName, clientName, serverName);

            if (!errorCode) {
                /* Create JID using the Windows logon name and user provided domain name */
                jid_ = JID(clientName, username);
            }
            else {
                loginWindow_->setMessage(str(format(QT_TRANSLATE_NOOP("", "Error obtaining Windows user name (%1%)")) % errorCode->message()));
                loginWindow_->setIsLoggingIn(false);
                return;
            }
        }
#endif

        loginWindow_->setMessage("");
        loginWindow_->setIsLoggingIn(true);
        profileSettings_ = new ProfileSettingsProvider(username, settings_);
        if (!settings_->getSetting(SettingConstants::FORGET_PASSWORDS)) {
            profileSettings_->storeString("jid", username);
            profileSettings_->storeString("certificate", certificatePath);
            profileSettings_->storeString("pass", (remember || loginAutomatically) ? password : "");
            std::string optionString = serializeClientOptions(options);
            profileSettings_->storeString("options", optionString);
            profileSettings_->storeInt("enabled", 1);
            loginWindow_->addAvailableAccount(profileSettings_->getStringSetting("jid"), profileSettings_->getStringSetting("pass"), profileSettings_->getStringSetting("certificate"), options);
        }

        password_ = password;
        certificate_ = certificate;
        clientOptions_ = options;
        performLoginFromCachedCredentials();
    }
}

void AccountController::handlePurgeSavedLoginRequest(const std::string& username) {
    settings_->removeProfile(username);
    loginWindow_->removeAvailableAccount(username);
}

void AccountController::performLoginFromCachedCredentials() {
    if (settings_->getSetting(SettingConstants::FORGET_PASSWORDS) && password_.empty()) {
        /* Then we can't try to login again. */
        return;
    }
    /* If we logged in with a bare JID, and we have a full bound JID, re-login with the
     * bound JID to try and keep dynamically assigned resources */
    JID clientJID = jid_;
    if (boundJID_.isValid() && jid_.isBare() && boundJID_.toBare() == jid_) {
        clientJID = boundJID_;
    }

    if (!statusTracker_) {
        statusTracker_  = new StatusTracker();
    }
    if (!clientInitialized_) {
        storages_ = storagesFactory_->createStorages(jid_.toBare());
        certificateStorage_ = certificateStorageFactory_->createCertificateStorage(jid_.toBare());
        certificateTrustChecker_ = new CertificateStorageTrustChecker(certificateStorage_);

        client_ = std::make_shared<Swift::Client>(clientJID, createSafeByteArray(password_.c_str()), networkFactories_, storages_);
        clientInitialized_ = true;
        client_->setCertificateTrustChecker(certificateTrustChecker_);
        client_->onDataRead.connect(boost::bind(&XMLConsoleController::handleDataRead, xmlConsoleController_, _1));
        client_->onDataWritten.connect(boost::bind(&XMLConsoleController::handleDataWritten, xmlConsoleController_, _1));
        client_->onDisconnected.connect(boost::bind(&AccountController::handleDisconnected, this, _1));
        client_->onConnected.connect(boost::bind(&AccountController::handleConnected, this));

        client_->setSoftwareVersion(CLIENT_NAME, buildVersion);

        client_->getVCardManager()->onVCardChanged.connect(boost::bind(&AccountController::handleVCardReceived, this, _1, _2));
        presenceNotifier_ = new PresenceNotifier(client_->getStanzaChannel(), notifier_, client_->getMUCRegistry(), client_->getAvatarManager(), client_->getNickResolver(), client_->getPresenceOracle(), networkFactories_->getTimerFactory());
        presenceNotifier_->onNotificationActivated.connect(boost::bind(&AccountController::handleNotificationClicked, this, _1));
        eventNotifier_ = new EventNotifier(eventController_, notifier_, client_->getAvatarManager(), client_->getNickResolver());
        eventNotifier_->onNotificationActivated.connect(boost::bind(&AccountController::handleNotificationClicked, this, _1));
        if (certificate_) {
            client_->setCertificate(certificate_);
        }
        std::shared_ptr<Presence> presence(new Presence());
        presence->setShow(static_cast<StatusShow::Type>(profileSettings_->getIntSetting("lastShow", StatusShow::Online)));
        presence->setStatus(profileSettings_->getStringSetting("lastStatus"));
        statusTracker_->setRequestedPresence(presence);
        fdpFormSubmitController_ = std::make_unique<FdpFormSubmitController>(jid_, client_->getIQRouter(), uiEventStream_, uiFactory_);
    } else {
        /* In case we're in the middle of another login, make sure they don't overlap */
        client_->disconnect();
    }
    systemTrayController_->setConnecting();
    if (rosterController_) {
        rosterController_->getWindow()->setConnecting();
    }
    ClientOptions clientOptions = clientOptions_;
    bool eagle = settings_->getSetting(SettingConstants::FORGET_PASSWORDS);
    clientOptions.forgetPassword = eagle;
    clientOptions.useTLS = eagle ? ClientOptions::RequireTLS : clientOptions_.useTLS;
    client_->connect(clientOptions);
}

void AccountController::handleDisconnected(const boost::optional<ClientError>& error) {
    if (rosterController_) {
        rosterController_->getWindow()->setStreamEncryptionStatus(false);
    }
    if (adHocManager_) {
        adHocManager_->setOnline(false);
    }
    if (settings_->getSetting(SettingConstants::FORGET_PASSWORDS)) {
        purgeCachedCredentials();
    }
    if (quitRequested_) {
        resetClient();
        loginWindow_->quit();
    }
    else if (error) {
        std::string message;
        std::string certificateErrorMessage;
        bool forceSignout = false;
        switch(error->getType()) {
            case ClientError::UnknownError: message = QT_TRANSLATE_NOOP("", "Unknown Error"); break;
            case ClientError::DomainNameResolveError: message = QT_TRANSLATE_NOOP("", "Unable to find server"); break;
            case ClientError::ConnectionError: message = QT_TRANSLATE_NOOP("", "Error connecting to server"); break;
            case ClientError::ConnectionReadError: message = QT_TRANSLATE_NOOP("", "Error while receiving server data"); break;
            case ClientError::ConnectionWriteError: message = QT_TRANSLATE_NOOP("", "Error while sending data to the server"); break;
            case ClientError::XMLError: message = QT_TRANSLATE_NOOP("", "Error parsing server data"); break;
            case ClientError::AuthenticationFailedError: message = QT_TRANSLATE_NOOP("", "Login/password invalid"); break;
            case ClientError::CompressionFailedError: message = QT_TRANSLATE_NOOP("", "Error while compressing stream"); break;
            case ClientError::ServerVerificationFailedError: message = QT_TRANSLATE_NOOP("", "Server verification failed"); break;
            case ClientError::NoSupportedAuthMechanismsError: message = QT_TRANSLATE_NOOP("", "Authentication mechanisms not supported"); break;
            case ClientError::UnexpectedElementError: message = QT_TRANSLATE_NOOP("", "Unexpected response"); break;
            case ClientError::ResourceBindError: message = QT_TRANSLATE_NOOP("", "Error binding resource"); break;
            case ClientError::SessionStartError: message = QT_TRANSLATE_NOOP("", "Error starting session"); break;
            case ClientError::StreamError: message = QT_TRANSLATE_NOOP("", "Stream error"); break;
            case ClientError::TLSError: message = QT_TRANSLATE_NOOP("", "Encryption error"); break;
            case ClientError::ClientCertificateLoadError: message = QT_TRANSLATE_NOOP("", "Error loading certificate (Invalid file or password?)"); break;
            case ClientError::ClientCertificateError: message = QT_TRANSLATE_NOOP("", "Certificate not authorized"); break;
            case ClientError::CertificateCardRemoved: message = QT_TRANSLATE_NOOP("", "Certificate card removed"); forceSignout = true; break;

            case ClientError::UnknownCertificateError: certificateErrorMessage = QT_TRANSLATE_NOOP("", "Unknown certificate"); break;
            case ClientError::CertificateExpiredError: certificateErrorMessage = QT_TRANSLATE_NOOP("", "Certificate has expired"); break;
            case ClientError::CertificateNotYetValidError: certificateErrorMessage = QT_TRANSLATE_NOOP("", "Certificate is not yet valid"); break;
            case ClientError::CertificateSelfSignedError: certificateErrorMessage = QT_TRANSLATE_NOOP("", "Certificate is self-signed"); break;
            case ClientError::CertificateRejectedError: certificateErrorMessage = QT_TRANSLATE_NOOP("", "Certificate has been rejected"); break;
            case ClientError::CertificateUntrustedError: certificateErrorMessage = QT_TRANSLATE_NOOP("", "Certificate is not trusted"); break;
            case ClientError::InvalidCertificatePurposeError: certificateErrorMessage = QT_TRANSLATE_NOOP("", "Certificate cannot be used for encrypting your connection"); break;
            case ClientError::CertificatePathLengthExceededError: certificateErrorMessage = QT_TRANSLATE_NOOP("", "Certificate path length constraint exceeded"); break;
            case ClientError::InvalidCertificateSignatureError: certificateErrorMessage = QT_TRANSLATE_NOOP("", "Invalid certificate signature"); break;
            case ClientError::InvalidCAError: certificateErrorMessage = QT_TRANSLATE_NOOP("", "Invalid Certificate Authority"); break;
            case ClientError::InvalidServerIdentityError: certificateErrorMessage = QT_TRANSLATE_NOOP("", "Certificate does not match the host identity"); break;
            case ClientError::RevokedError: certificateErrorMessage = QT_TRANSLATE_NOOP("", "Certificate has been revoked"); break;
            case ClientError::RevocationCheckFailedError: certificateErrorMessage = QT_TRANSLATE_NOOP("", "Unable to determine certificate revocation state"); break;
        }
        bool forceReconnectAfterCertificateTrust = false;
        if (!certificateErrorMessage.empty()) {
            std::vector<Certificate::ref> certificates = certificateTrustChecker_->getLastCertificateChain();
            if (!certificates.empty() && loginWindow_->askUserToTrustCertificatePermanently(certificateErrorMessage, certificates)) {
                certificateStorage_->addCertificate(certificates[0]);
                forceReconnectAfterCertificateTrust = true;
            }
            else {
                message = QT_TRANSLATE_NOOP("", "Certificate error");
            }
        }

        if (!message.empty() && error->getErrorCode()) {
            message = str(format(QT_TRANSLATE_NOOP("", "%1% (%2%)")) % message % error->getErrorCode()->message());
        }

        if (forceReconnectAfterCertificateTrust && settings_->getSetting(SettingConstants::FORGET_PASSWORDS)) {
            forceReconnectAfterCertificateTrust = false;
            forceSignout = true;
            message = QT_TRANSLATE_NOOP("", "Re-enter credentials and retry");
        }

        if (forceReconnectAfterCertificateTrust) {
            performLoginFromCachedCredentials();
        }
        else if (forceSignout || !rosterController_) { //hasn't been logged in yet or permanent error
            signOut();
            loginWindow_->setMessage(message);
            loginWindow_->setIsLoggingIn(false);
        } else {
            logout();
            if (settings_->getSetting(SettingConstants::FORGET_PASSWORDS)) {
                message = str(format(QT_TRANSLATE_NOOP("", "Disconnected from %1%: %2%. To reconnect, Sign Out and provide your password again.")) % jid_.getDomain() % message);
            } else {
                if (!offlineRequested_) {
                    setReconnectTimer();
                }
                if (lastDisconnectError_) {
                    message = str(format(QT_TRANSLATE_NOOP("", "Reconnect to %1% failed: %2%. Will retry in %3% seconds.")) % jid_.getDomain() % message % std::to_string(timeBeforeNextReconnect_));
                    lastDisconnectError_->conclude();
                } else {
                    message = str(format(QT_TRANSLATE_NOOP("", "Disconnected from %1%: %2%.")) % jid_.getDomain() % message);
                }
                lastDisconnectError_ = std::make_shared<ErrorEvent>(JID(jid_.getDomain()), message);
                eventController_->handleIncomingEvent(lastDisconnectError_);
            }
        }
    }
    else if (!rosterController_) { //hasn't been logged in yet
        loginWindow_->setIsLoggingIn(false);
    }
}

void AccountController::setReconnectTimer() {
    if (timeBeforeNextReconnect_ < 0) {
        timeBeforeNextReconnect_ = 1;
    } else {
        timeBeforeNextReconnect_ = timeBeforeNextReconnect_ >= 150 ? 300 : timeBeforeNextReconnect_ * 2; // Randomly selected by roll of a die, as required by 3920bis
    }
    if (reconnectTimer_) {
        reconnectTimer_->stop();
    }
    reconnectTimer_ = networkFactories_->getTimerFactory()->createTimer(timeBeforeNextReconnect_ * 1000);
    reconnectTimer_->onTick.connect(boost::bind(&AccountController::reconnectAfterError, this));
    reconnectTimer_->start();
}

void AccountController::handleCancelLoginRequest() {
    signOut();
}

void AccountController::signOut() {
    if (settings_->getSetting(SettingConstants::FORGET_PASSWORDS)) {
        purgeCachedCredentials();
    }
    profileSettings_->storeInt("enabled", 0);
    eventController_->clear();
    logout();
    loginWindow_->loggedOut();
    resetClient();
}

void AccountController::logout() {
    if (settings_->getSetting(SettingConstants::FORGET_PASSWORDS)) {
        purgeCachedCredentials();
    }
    systemTrayController_->setMyStatusType(StatusShow::None);
    if (clientInitialized_ /*&& client_->isAvailable()*/) {
        client_->disconnect();
    }
    if (rosterController_ && myStatusLooksOnline_) {
        rosterController_->getWindow()->setMyStatusType(StatusShow::None);
        rosterController_->getWindow()->setMyStatusText("");
        myStatusLooksOnline_ = false;
    }
    setManagersOffline();
}

void AccountController::setManagersOffline() {
    if (chatsManager_) {
        chatsManager_->setOnline(false);
    }
    if (rosterController_) {
        rosterController_->setEnabled(false);
    }
    if (profileController_) {
        profileController_->setAvailable(false);
    }
    if (contactEditController_) {
        contactEditController_->setAvailable(false);
    }
}

void AccountController::handleServerDiscoInfoResponse(std::shared_ptr<DiscoInfo> info, ErrorPayload::ref error) {
    if (!error) {
        chatsManager_->setServerDiscoInfo(info);
        adHocManager_->setServerDiscoInfo(info);
        if (info->hasFeature(DiscoInfo::BlockingCommandFeature)) {
            rosterController_->getWindow()->setBlockingCommandAvailable(true);
            rosterController_->initBlockingCommand();
        }
        if (info->hasFeature(DiscoInfo::MessageCarbonsFeature)) {
            enableMessageCarbons();
        }
    }
}

void AccountController::enableMessageCarbons() {
    auto enableCarbonsRequest = EnableCarbonsRequest::create(client_->getIQRouter());
    enableCarbonsRequestHandlerConnection_ = enableCarbonsRequest->onResponse.connect([&](Payload::ref /*payload*/, ErrorPayload::ref error) {
        if (error) {
            SWIFT_LOG(warning) << "Failed to enable carbons." << std::endl;
        }
        else {
            SWIFT_LOG(debug) << "Successfully enabled carbons." << std::endl;
        }
        enableCarbonsRequestHandlerConnection_.disconnect();
    });
    enableCarbonsRequest->send();
}

void AccountController::handleVCardReceived(const JID& jid, VCard::ref vCard) {
    if (!jid.equals(jid_, JID::WithoutResource) || !vCard) {
        return;
    }
    std::string hash;
    if (!vCard->getPhoto().empty()) {
        hash = Hexify::hexify(networkFactories_->getCryptoProvider()->getSHA1Hash(vCard->getPhoto()));
    }
    if (hash != vCardPhotoHash_) {
        vCardPhotoHash_ = hash;
        if (client_ && client_->isAvailable()) {
            sendPresence(statusTracker_->getNextPresence());
        }
    }
}

void AccountController::handleNotificationClicked(const JID& jid) {
    assert(chatsManager_);
    if (clientInitialized_) {
        if (client_->getMUCRegistry()->isMUC(jid)) {
            uiEventStream_->send(std::make_shared<JoinMUCUIEvent>(jid));
        }
        else {
            uiEventStream_->send(std::make_shared<RequestChatUIEvent>(jid));
        }
    }
}

void AccountController::handleQuitRequest() {
    if (client_ && client_->isActive()) {
        quitRequested_ = true;
        client_->disconnect();
    }
    else {
        resetClient();
        loginWindow_->quit();
    }
}

//FIXME: Switch all this to boost::serialise

#define SERIALIZE_BOOL(option) result += options.option ? "1" : "0"; result += ",";
#define SERIALIZE_INT(option) result += boost::lexical_cast<std::string>(options.option); result += ",";
#define SERIALIZE_STRING(option) result += Base64::encode(createByteArray(options.option)); result += ",";
#define SERIALIZE_SAFE_STRING(option) result += safeByteArrayToString(Base64::encode(options.option)); result += ",";
#define SERIALIZE_URL(option) SERIALIZE_STRING(option.toString())

std::string AccountController::serializeClientOptions(const ClientOptions& options) {
    std::string result;
    SERIALIZE_BOOL(useStreamCompression);
    switch (options.useTLS) {
        case ClientOptions::NeverUseTLS: result += "1";break;
        case ClientOptions::UseTLSWhenAvailable: result += "2";break;
        case ClientOptions::RequireTLS: result += "3";break;
    }
    result += ",";
    SERIALIZE_BOOL(allowPLAINWithoutTLS);
    SERIALIZE_BOOL(useStreamResumption);
    SERIALIZE_BOOL(useAcks);
    SERIALIZE_STRING(manualHostname);
    SERIALIZE_INT(manualPort);
    switch (options.proxyType) {
        case ClientOptions::NoProxy: result += "1";break;
        case ClientOptions::SystemConfiguredProxy: result += "2";break;
        case ClientOptions::SOCKS5Proxy: result += "3";break;
        case ClientOptions::HTTPConnectProxy: result += "4";break;
    }
    result += ",";
    SERIALIZE_STRING(manualProxyHostname);
    SERIALIZE_INT(manualProxyPort);
    SERIALIZE_URL(boshURL);
    SERIALIZE_URL(boshHTTPConnectProxyURL);
    SERIALIZE_SAFE_STRING(boshHTTPConnectProxyAuthID);
    SERIALIZE_SAFE_STRING(boshHTTPConnectProxyAuthPassword);
    SERIALIZE_BOOL(tlsOptions.schannelTLS1_0Workaround);
    SERIALIZE_BOOL(singleSignOn);
    return result;
}

}
