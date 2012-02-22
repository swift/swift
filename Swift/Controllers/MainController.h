/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "Swiften/Network/Timer.h"
#include <string>
#include "Swiften/Client/ClientError.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/Elements/VCard.h"
#include "Swiften/Elements/ErrorPayload.h"
#include "Swiften/Elements/Presence.h"
#include "Swift/Controllers/Settings/SettingsProvider.h"
#include "Swift/Controllers/ProfileSettingsProvider.h"
#include "Swiften/Elements/CapsInfo.h"
#include "Swift/Controllers/XMPPEvents/ErrorEvent.h"
#include "Swift/Controllers/UIEvents/UIEvent.h"
#include "Swiften/Client/ClientXMLTracer.h"

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
	class ContactEditController;
	class TogglableNotifier;
	class PresenceNotifier;
	class EventNotifier;
	class SystemTray;
	class SystemTrayController;
	class SoundEventController;
	class SoundPlayer;
	class XMLConsoleController;
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
					bool useDelayForLatency);
			~MainController();


		private:
			void resetClient();
			void handleConnected();
			void handleLoginRequest(const std::string& username, const std::string& password, const std::string& certificatePath, CertificateWithKey::ref certificate, bool remember, bool loginAutomatically);
			void handleCancelLoginRequest();
			void handleQuitRequest();
			void handleChangeStatusRequest(StatusShow::Type show, const std::string &statusText);
			void handleDisconnected(const boost::optional<ClientError>& error);
			void handleServerDiscoInfoResponse(boost::shared_ptr<DiscoInfo>, ErrorPayload::ref);
			void handleEventQueueLengthChange(int count);
			void handleVCardReceived(const JID& j, VCard::ref vCard);
			void handleSettingChanged(const std::string& settingPath);
			void handlePurgeSavedLoginRequest(const std::string& username);
			void sendPresence(boost::shared_ptr<Presence> presence);
			void handleInputIdleChanged(bool);
			void logout();
			void signOut();
			void setReconnectTimer();
			void resetPendingReconnects();
			void resetCurrentError();
	
			void performLoginFromCachedCredentials();
			void reconnectAfterError();
			void setManagersOffline();
			void handleNotificationClicked(const JID& jid);
			void handleForceQuit();
			void purgeCachedCredentials();

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
			boost::shared_ptr<Client> client_;
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
			FileTransferListController* fileTransferListController_;
			ChatsManager* chatsManager_;
			ProfileController* profileController_;
			ContactEditController* contactEditController_;
			JID jid_;
			JID boundJID_;
			SystemTrayController* systemTrayController_;
			SoundEventController* soundEventController_;
			XMPPURIController* xmppURIController_;
			std::string vCardPhotoHash_;
			std::string password_;
			CertificateWithKey::ref certificate_;
			boost::shared_ptr<ErrorEvent> lastDisconnectError_;
			bool useDelayForLatency_;
			UserSearchController* userSearchControllerChat_;
			UserSearchController* userSearchControllerAdd_;
			int timeBeforeNextReconnect_;
			Timer::ref reconnectTimer_;
			StatusTracker* statusTracker_;
			bool myStatusLooksOnline_;
			bool quitRequested_;
			bool offlineRequested_;
			static const int SecondsToWaitBeforeForceQuitting;
			FileTransferOverview* ftOverview_;
	};
}
