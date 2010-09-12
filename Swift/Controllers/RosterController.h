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
#include "Swiften/Elements/ErrorPayload.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Avatars/AvatarManager.h"
#include "Swift/Controllers/UIEvents/UIEvent.h"

#include "Swiften/Base/boost_bsignals.h"
#include <boost/shared_ptr.hpp>

namespace Swift {
	class IQRouter;
	class Roster;
	class XMPPRoster;
	class MainWindow;
	class MainWindowFactory;
	class OfflineRosterFilter;
	class NickResolver;
	class PresenceOracle;
	class PresenceSender;
	class EventController;
	class SubscriptionRequestEvent;
	class UIEventStream;
	class IQRouter;

	class RosterController {
		public:
			RosterController(const JID& jid, XMPPRoster* xmppRoster, AvatarManager* avatarManager, MainWindowFactory* mainWindowFactory, NickResolver* nickResolver, PresenceOracle* presenceOracle, PresenceSender* presenceSender, EventController* eventController, UIEventStream* uiEventStream, IQRouter* iqRouter_);
			~RosterController();
			void showRosterWindow();
			MainWindow* getWindow() {return mainWindow_;};
			void setNickResolver(NickResolver* nickResolver);
			boost::signal<void (StatusShow::Type, const String&)> onChangeStatusRequest;
			boost::signal<void ()> onSignOutRequest;
			void handleAvatarChanged(const JID& jid);
			void setEnabled(bool enabled);
		private:
			void handleOnJIDAdded(const JID &jid);
			void handleRosterCleared();
			void handleOnJIDRemoved(const JID &jid);
			void handleOnJIDUpdated(const JID &jid, const String& oldName, const std::vector<String> oldGroups);
			void handleStartChatRequest(const JID& contact);
			void handleChangeStatusRequest(StatusShow::Type show, const String &statusText);
			void handleShowOfflineToggled(bool state);
			void handleIncomingPresence(boost::shared_ptr<Presence> newPresence);
			void handleSubscriptionRequest(const JID& jid, const String& message);
			void handleSubscriptionRequestAccepted(SubscriptionRequestEvent* event);
			void handleSubscriptionRequestDeclined(SubscriptionRequestEvent* event);
			void handleUIEvent(boost::shared_ptr<UIEvent> event);
			void handleRosterSetError(boost::optional<ErrorPayload> error, boost::shared_ptr<RosterPayload> rosterPayload);
			void handleOwnNickChanged(const String& nick);
			JID myJID_;
			XMPPRoster* xmppRoster_;
			MainWindowFactory* mainWindowFactory_;
			MainWindow* mainWindow_;
			Roster* roster_;
			OfflineRosterFilter* offlineFilter_;
			AvatarManager* avatarManager_;
			NickResolver* nickResolver_;
			PresenceOracle* presenceOracle_;
			PresenceSender* presenceSender_;
			EventController* eventController_;
			IQRouter* iqRouter_;
			boost::bsignals::scoped_connection changeStatusConnection_;
			boost::bsignals::scoped_connection showOfflineConnection_;
			boost::bsignals::scoped_connection signOutConnection_;
			boost::bsignals::scoped_connection uiEventConnection_;
	};
}
#endif

