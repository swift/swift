/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <boost/signals2.hpp>

#include <Swiften/Client/ClientError.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/Elements/CapsInfo.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Network/Timer.h>

#include <Swift/Controllers/ProfileSettingsProvider.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swift/Controllers/UIEvents/UIEvent.h>
#include <Swift/Controllers/XMPPEvents/ErrorEvent.h>

namespace Swift {
    class IdleDetector;
    class UIFactory;
    class EventLoop;
    class Client;
    class ChatController;
    class ChatsManager;
    class CertificateStorageFactory;
    class CertificateStorage;
    class CertificateStorageTrustChecker;
    class EventController;
    class MainWindow;
    class RosterController;
    class LoginWindow;
    class EventLoop;
    class MUCController;
    class Notifier;
    class ProfileController;
    class ShowProfileController;
    class ContactEditController;
    class TogglableNotifier;
    class PresenceNotifier;
    class EventNotifier;
    class SystemTray;
    class SystemTrayController;
    class SoundEventController;
    class SoundPlayer;
    class XMLConsoleController;
    class HistoryViewController;
    class HistoryController;
    class FileTransferListController;
    class UIEventStream;
    class EventWindowFactory;
    class EventWindowController;
    class MUCSearchController;
    class UserSearchController;
    class StatusTracker;
    class Dock;
    class Storages;
    class StoragesFactory;
    class NetworkFactories;
    class URIHandler;
    class XMPPURIController;
    class AdHocManager;
    class AdHocCommandWindowFactory;
    class FileTransferOverview;
    class WhiteboardManager;
    class HighlightManager;
    class HighlightEditorController;
    class BlockListController;
    class ContactSuggester;
    class ContactsFromXMPPRoster;

    class MainController {
        public:
            MainController(
                    EventLoop* eventLoop,
                    NetworkFactories* networkFactories,
                    UIFactory* uiFactories,
                    SettingsProvider *settings,
                    SystemTray* systemTray,
                    SoundPlayer* soundPlayer,
                    StoragesFactory* storagesFactory,
                    CertificateStorageFactory* certificateStorageFactory,
                    Dock* dock,
                    Notifier* notifier,
                    URIHandler* uriHandler,
                    IdleDetector* idleDetector,
                    const std::map<std::string, std::string>& emoticons,
                    bool useDelayForLatency);
            ~MainController();


        private:
            void resetClient();
            void handleConnected();
            void handleLoginRequest(const std::string& username, const std::string& password, const std::string& certificatePath, CertificateWithKey::ref certificate, const ClientOptions& options, bool remember, bool loginAutomatically);
            void handleCancelLoginRequest();
            void handleQuitRequest();
            void handleChangeStatusRequest(StatusShow::Type show, const std::string &statusText);
            void handleDisconnected(const boost::optional<ClientError>& error);
            void handleServerDiscoInfoResponse(std::shared_ptr<DiscoInfo>, ErrorPayload::ref);
            void handleEventQueueLengthChange(int count);
            void handleVCardReceived(const JID& j, VCard::ref vCard);
            void handleSettingChanged(const std::string& settingPath);
            void handlePurgeSavedLoginRequest(const std::string& username);
            void sendPresence(std::shared_ptr<Presence> presence);
            void handleInputIdleChanged(bool);
            void handleShowCertificateRequest();
            void logout();
            void signOut();
            void setReconnectTimer();
            void resetPendingReconnects();
            void resetCurrentError();
            void enableMessageCarbons();

            void performLoginFromCachedCredentials();
            void reconnectAfterError();
            void setManagersOffline();
            void handleNotificationClicked(const JID& jid);
            void handleForceQuit();
            void purgeCachedCredentials();
            std::string serializeClientOptions(const ClientOptions& options);
            ClientOptions parseClientOptions(const std::string& optionString);

        private:
            EventLoop* eventLoop_;
            NetworkFactories* networkFactories_;
            UIFactory* uiFactory_;
            StoragesFactory* storagesFactory_;
            Storages* storages_;
            CertificateStorageFactory* certificateStorageFactory_;
            CertificateStorage* certificateStorage_;
            CertificateStorageTrustChecker* certificateTrustChecker_;
            bool clientInitialized_;
            std::shared_ptr<Client> client_;
            SettingsProvider *settings_;
            ProfileSettingsProvider* profileSettings_;
            Dock* dock_;
            URIHandler* uriHandler_;
            IdleDetector* idleDetector_;
            TogglableNotifier* notifier_;
            PresenceNotifier* presenceNotifier_;
            EventNotifier* eventNotifier_;
            RosterController* rosterController_;
            EventController* eventController_;
            EventWindowController* eventWindowController_;
            AdHocManager* adHocManager_;
            LoginWindow* loginWindow_;
            UIEventStream* uiEventStream_;
            XMLConsoleController* xmlConsoleController_;
            HistoryViewController* historyViewController_;
            HistoryController* historyController_;
            FileTransferListController* fileTransferListController_;
            BlockListController* blockListController_;
            ChatsManager* chatsManager_;
            ProfileController* profileController_;
            ShowProfileController* showProfileController_;
            ContactEditController* contactEditController_;
            ContactsFromXMPPRoster* contactsFromRosterProvider_;
            ContactSuggester* contactSuggesterWithoutRoster_;
            ContactSuggester* contactSuggesterWithRoster_;
            JID jid_;
            JID boundJID_;
            SystemTrayController* systemTrayController_;
            SoundEventController* soundEventController_;
            XMPPURIController* xmppURIController_;
            std::string vCardPhotoHash_;
            std::string password_;
            CertificateWithKey::ref certificate_;
            ClientOptions clientOptions_;
            std::shared_ptr<ErrorEvent> lastDisconnectError_;
            bool useDelayForLatency_;
            UserSearchController* userSearchControllerChat_;
            UserSearchController* userSearchControllerAdd_;
            UserSearchController* userSearchControllerInvite_;
            int timeBeforeNextReconnect_;
            Timer::ref reconnectTimer_;
            StatusTracker* statusTracker_;
            bool myStatusLooksOnline_ = false;
            bool quitRequested_;
            bool offlineRequested_;
            static const int SecondsToWaitBeforeForceQuitting;
            FileTransferOverview* ftOverview_;
            WhiteboardManager* whiteboardManager_;
            HighlightManager* highlightManager_;
            HighlightEditorController* highlightEditorController_;
            std::map<std::string, std::string> emoticons_;
            boost::signals2::connection enableCarbonsRequestHandlerConnection_;
    };
}
