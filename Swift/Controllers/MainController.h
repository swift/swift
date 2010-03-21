#pragma once

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "Swiften/Network/BoostIOServiceThread.h"
#include "Swiften/Network/BoostTimerFactory.h"
#include "SwifTools/Idle/PlatformIdleQuerier.h"
#include "SwifTools/Idle/ActualIdleDetector.h"
#include "Swiften/Base/String.h"
#include "Swiften/Client/ClientError.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/VCard.h"
#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/Elements/ErrorPayload.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Settings/SettingsProvider.h"
#include "Swiften/Elements/CapsInfo.h"
#include "Swiften/Roster/XMPPRoster.h"

namespace Swift {
	class AvatarStorage;
	class Application;
	class Client;
	class ChatWindowFactory;
	class ChatController;
	class ChatsManager;
	class EventController;
	class MainWindowFactory;
	class MainWindow;
	class NickResolver;
	class RosterController;
	class XMPPRosterController;
	class PresenceSender;
	class DiscoInfoResponder;
	class AvatarManager;
	class LoginWindow;
	class EventLoop;
	class SoftwareVersionResponder;
	class LoginWindowFactory;
	class TreeWidgetFactory;
	class MUCController;
	class PresenceOracle;
	class SystemTray;
	class SystemTrayController;
	class SoundEventController;
	class SoundPlayer;
	class XMLConsoleController;
	class UIEventStream;
	class XMLConsoleWidgetFactory;
	class EventWindowFactory;
	class EventWindowController;

	class MainController {
		public:
			MainController(ChatWindowFactory* chatWindowFactory, MainWindowFactory *mainWindowFactory, LoginWindowFactory *loginWindowFactory, TreeWidgetFactory* treeWidgetFactory, EventWindowFactory* eventWindowFactory, SettingsProvider *settings, Application* application, SystemTray* systemTray, SoundPlayer* soundPlayer, XMLConsoleWidgetFactory* xmlConsoleWidgetFactory);
			~MainController();


		private:
			void resetClient();
			void handleConnected();
			void handleLoginRequest(const String& username, const String& password, const String& certificateFile, bool remember);
			void handleCancelLoginRequest();
			void handleChangeStatusRequest(StatusShow::Type show, const String &statusText);
			void handleError(const ClientError& error);
			void handleServerDiscoInfoResponse(boost::shared_ptr<DiscoInfo>, const boost::optional<ErrorPayload>&);
			void handleEventQueueLengthChange(int count);
			void handleOwnVCardReceived(boost::shared_ptr<VCard> vCard, const boost::optional<ErrorPayload>& error);
			void sendPresence(boost::shared_ptr<Presence> presence);
			void handleInputIdleChanged(bool);
			void logout();
			void signOut();
	
			void performLoginFromCachedCredentials();
			void reconnectAfterError();
			void setManagersEnabled(bool enabled);

			BoostIOServiceThread boostIOServiceThread_;
			BoostTimerFactory timerFactory_;
			PlatformIdleQuerier idleQuerier_;
			ActualIdleDetector idleDetector_;
			Client* client_;
			PresenceSender* presenceSender_;
			ChatWindowFactory* chatWindowFactory_;
			MainWindowFactory* mainWindowFactory_;
			LoginWindowFactory* loginWindowFactory_;
			TreeWidgetFactory* treeWidgetFactory_;
			EventWindowFactory* eventWindowFactory_;
			SettingsProvider *settings_;
			Application* application_;
			AvatarStorage* avatarStorage_;
			ChatController* chatController_;
			XMPPRosterController* xmppRosterController_;
			RosterController* rosterController_;
			EventController* eventController_;
			EventWindowController* eventWindowController_;
			LoginWindow* loginWindow_;
			SoftwareVersionResponder* clientVersionResponder_;
			NickResolver* nickResolver_;
			DiscoInfoResponder* discoResponder_;
			UIEventStream* uiEventStream_;
			XMLConsoleController* xmlConsoleController_;
			ChatsManager* chatsManager_;
			boost::shared_ptr<CapsInfo> capsInfo_;
			boost::shared_ptr<DiscoInfo> serverDiscoInfo_;
			boost::shared_ptr<XMPPRoster> xmppRoster_;;
			JID jid_;
			PresenceOracle* presenceOracle_;
			SystemTrayController* systemTrayController_;
			SoundEventController* soundEventController_;
			AvatarManager* avatarManager_;
			boost::shared_ptr<Presence> lastSentPresence_;
			boost::shared_ptr<Presence> preIdlePresence_;
			String vCardPhotoHash_;
			boost::shared_ptr<Presence> queuedPresence_;
			String password_;
			String certificateFile_;
	};
}
