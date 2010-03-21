#ifndef SWIFTEN_RosterController_H
#define SWIFTEN_RosterController_H

#include "Swiften/JID/JID.h"
#include "Swiften/Base/String.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Roster/UserRosterAction.h"
#include "Swiften/Avatars/AvatarManager.h"

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class IQRouter;
	class Roster;
	class XMPPRoster;
	class MainWindow;
	class MainWindowFactory;
	class TreeWidgetFactory;
	class OfflineRosterFilter;
	class NickResolver;
	class PresenceOracle;
	class EventController;
	class SubscriptionRequestEvent;
	class UIEventStream;

	class RosterController {
		public:
			RosterController(const JID& jid, boost::shared_ptr<XMPPRoster> xmppRoster, AvatarManager* avatarManager, MainWindowFactory* mainWindowFactory, TreeWidgetFactory* treeWidgetFactory, NickResolver* nickResolver, PresenceOracle* presenceOracle, EventController* eventController, UIEventStream* uiEventStream);
			~RosterController();
			void showRosterWindow();
			MainWindow* getWindow() {return mainWindow_;};
			void setAvatarManager(AvatarManager* avatarManager);
			void setNickResolver(NickResolver* nickResolver);
			boost::signal<void (const JID&)> onStartChatRequest;
			boost::signal<void (const JID&, const String&)> onJoinMUCRequest;
			boost::signal<void (StatusShow::Type, const String&)> onChangeStatusRequest;
			boost::signal<void ()> onSignOutRequest;
			void handleAvatarChanged(const JID& jid, const String& hash);
			void setEnabled(bool enabled);
		private:
			void handleOnJIDAdded(const JID &jid);
			void handleOnJIDRemoved(const JID &jid);
			void handleOnJIDUpdated(const JID &jid, const String& oldName, const std::vector<String> oldGroups);
			void handleStartChatRequest(const JID& contact);
			void handleJoinMUCRequest(const JID &muc, const String &nick);
			void handleUserAction(boost::shared_ptr<UserRosterAction> action);
			void handleChangeStatusRequest(StatusShow::Type show, const String &statusText);
			void handleShowOfflineToggled(bool state);
			void handleIncomingPresence(boost::shared_ptr<Presence> newPresence, boost::shared_ptr<Presence> oldPresence);
			void handleSubscriptionRequest(const JID& jid, const String& message);
			void handleSubscriptionRequestAccepted(SubscriptionRequestEvent* event);
			void handleSubscriptionRequestDeclined(SubscriptionRequestEvent* event);
			JID myJID_;
			boost::shared_ptr<XMPPRoster> xmppRoster_;
			MainWindowFactory* mainWindowFactory_;
			TreeWidgetFactory* treeWidgetFactory_;
			MainWindow* mainWindow_;
			Roster* roster_;
			OfflineRosterFilter* offlineFilter_;
			AvatarManager* avatarManager_;
			NickResolver* nickResolver_;
			PresenceOracle* presenceOracle_;
			EventController* eventController_;
	};
}
#endif

