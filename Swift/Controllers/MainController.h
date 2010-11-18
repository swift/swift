/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "Swiften/Network/Timer.h"
#include "SwifTools/Idle/PlatformIdleQuerier.h"
#include "SwifTools/Idle/ActualIdleDetector.h"
#include "Swiften/Base/String.h"
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

namespace Swift {
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
	class TogglableNotifier;
	class PresenceNotifier;
	class EventNotifier;
	class SystemTray;
	class SystemTrayController;
	class SoundEventController;
	class SoundPlayer;
	class XMLConsoleController;
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
					bool useDelayForLatency);
			~MainController();


		private:
			void resetClient();
			void handleConnected();
			void handleLoginRequest(const String& username, const String& password, const String& certificateFile, bool remember, bool loginAutomatically);
			void handleCancelLoginRequest();
			void handleQuitRequest();
			void handleChangeStatusRequest(StatusShow::Type show, const String &statusText);
			void handleDisconnected(const boost::optional<ClientError>& error);
			void handleServerDiscoInfoResponse(boost::shared_ptr<DiscoInfo>, ErrorPayload::ref);
			void handleEventQueueLengthChange(int count);
			void handleVCardReceived(const JID& j, VCard::ref vCard);
			void handleUIEvent(boost::shared_ptr<UIEvent> event);
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

		private:
			EventLoop* eventLoop_;
			NetworkFactories* networkFactories_;
			UIFactory* uiFactory_;
			PlatformIdleQuerier idleQuerier_;
			ActualIdleDetector idleDetector_;
			StoragesFactory* storagesFactory_;
			Storages* storages_;
			CertificateStorageFactory* certificateStorageFactory_;
			CertificateStorage* certificateStorage_;
			CertificateStorageTrustChecker* certificateTrustChecker_;
			Client* client_;
			SettingsProvider *settings_;
			ProfileSettingsProvider* profileSettings_;
			Dock* dock_;
			TogglableNotifier* notifier_;
			PresenceNotifier* presenceNotifier_;
			EventNotifier* eventNotifier_;
			RosterController* rosterController_;
			EventController* eventController_;
			EventWindowController* eventWindowController_;
			LoginWindow* loginWindow_;
			UIEventStream* uiEventStream_;
			XMLConsoleController* xmlConsoleController_;
			ChatsManager* chatsManager_;
			JID jid_;
			JID boundJID_;
			SystemTrayController* systemTrayController_;
			SoundEventController* soundEventController_;
			String vCardPhotoHash_;
			String password_;
			String certificateFile_;
			boost::shared_ptr<ErrorEvent> lastDisconnectError_;
			bool useDelayForLatency_;
			MUCSearchController* mucSearchController_;
			UserSearchController* userSearchController_;
			int timeBeforeNextReconnect_;
			Timer::ref reconnectTimer_;
			StatusTracker* statusTracker_;
			bool myStatusLooksOnline_;
			bool quitRequested_;
	};
}
