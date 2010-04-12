/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_RosterController_H
#define SWIFTEN_RosterController_H

#include "Swiften/JID/JID.h"
#include "Swiften/Base/String.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Roster/UserRosterAction.h"
#include "Swiften/Avatars/AvatarManager.h"
#include "Swift/Controllers/UIEvents/UIEvent.h"

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
	class IQRouter;

	class RosterController {
		public:
			RosterController(const JID& jid, boost::shared_ptr<XMPPRoster> xmppRoster, AvatarManager* avatarManager, MainWindowFactory* mainWindowFactory, TreeWidgetFactory* treeWidgetFactory, NickResolver* nickResolver, PresenceOracle* presenceOracle, EventController* eventController, UIEventStream* uiEventStream, IQRouter* iqRouter_);
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
			void handleUIEvent(boost::shared_ptr<UIEvent> event);
			void handleRosterSetError(boost::optional<ErrorPayload> error, boost::shared_ptr<RosterPayload> rosterPayload);
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
			IQRouter* iqRouter_;
			boost::bsignals::scoped_connection joinMUCConnection_;
			boost::bsignals::scoped_connection changeStatusConnection_;
			boost::bsignals::scoped_connection showOfflineConnection_;
			boost::bsignals::scoped_connection signOutConnection_;
			boost::bsignals::scoped_connection uiEventConnection_;
	};
}
#endif

