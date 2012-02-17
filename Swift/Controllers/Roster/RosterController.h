/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/JID/JID.h"
#include <string>
#include <set>
#include "Swiften/Elements/Presence.h"
#include "Swiften/Elements/ErrorPayload.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Avatars/AvatarManager.h"
#include "Swift/Controllers/UIEvents/UIEvent.h"
#include "RosterGroupExpandinessPersister.h"
#include "Swift/Controllers/FileTransfer/FileTransferOverview.h"

#include "Swiften/Base/boost_bsignals.h"
#include <boost/shared_ptr.hpp>

namespace Swift {
	class IQRouter;
	class Roster;
	class XMPPRoster;
	class XMPPRosterItem;
	class MainWindow;
	class MainWindowFactory;
	class OfflineRosterFilter;
	class NickResolver;
	class PresenceOracle;
	class SubscriptionManager;
	class EventController;
	class SubscriptionRequestEvent;
	class UIEventStream;
	class IQRouter;
	class SettingsProvider;
	class NickManager;
	class EntityCapsProvider;
	class FileTransferManager;
	
	class RosterController {
		public:
			RosterController(const JID& jid, XMPPRoster* xmppRoster, AvatarManager* avatarManager, MainWindowFactory* mainWindowFactory, NickManager* nickManager, NickResolver* nickResolver, PresenceOracle* presenceOracle, SubscriptionManager* subscriptionManager, EventController* eventController, UIEventStream* uiEventStream, IQRouter* iqRouter_, SettingsProvider* settings, EntityCapsProvider* entityCapsProvider, FileTransferOverview* fileTransferOverview);
			~RosterController();
			void showRosterWindow();
			MainWindow* getWindow() {return mainWindow_;};
			boost::signal<void (StatusShow::Type, const std::string&)> onChangeStatusRequest;
			boost::signal<void ()> onSignOutRequest;
			void handleAvatarChanged(const JID& jid);
			void setEnabled(bool enabled);

			boost::optional<XMPPRosterItem> getItem(const JID&) const;
			std::set<std::string> getGroups() const;

			void setContactGroups(const JID& jid, const std::vector<std::string>& groups);
			void updateItem(const XMPPRosterItem&);

		private:
			void handleOnJIDAdded(const JID &jid);
			void handleRosterCleared();
			void handleOnJIDRemoved(const JID &jid);
			void handleOnJIDUpdated(const JID &jid, const std::string& oldName, const std::vector<std::string>& oldGroups);
			void handleStartChatRequest(const JID& contact);
			void handleChangeStatusRequest(StatusShow::Type show, const std::string &statusText);
			void handleShowOfflineToggled(bool state);
			void handleIncomingPresence(boost::shared_ptr<Presence> newPresence);
			void handleSubscriptionRequest(const JID& jid, const std::string& message);
			void handleSubscriptionRequestAccepted(SubscriptionRequestEvent* event);
			void handleSubscriptionRequestDeclined(SubscriptionRequestEvent* event);
			void handleUIEvent(boost::shared_ptr<UIEvent> event);
			void handleRosterSetError(ErrorPayload::ref error, boost::shared_ptr<RosterPayload> rosterPayload);
			void applyAllPresenceTo(const JID& jid);
			void handleEditProfileRequest();
			void handleOnCapsChanged(const JID& jid);
			void handleSettingChanged(const std::string& settingPath);

			JID myJID_;
			XMPPRoster* xmppRoster_;
			MainWindowFactory* mainWindowFactory_;
			MainWindow* mainWindow_;
			Roster* roster_;
			OfflineRosterFilter* offlineFilter_;
			AvatarManager* avatarManager_;
			NickManager* nickManager_;
			NickResolver* nickResolver_;
			PresenceOracle* presenceOracle_;
			SubscriptionManager* subscriptionManager_;
			EventController* eventController_;
			RosterGroupExpandinessPersister* expandiness_;
			IQRouter* iqRouter_;
			SettingsProvider* settings_;
			UIEventStream* uiEventStream_;
			EntityCapsProvider* entityCapsManager_;
			FileTransferOverview* ftOverview_;
			
			boost::bsignals::scoped_connection changeStatusConnection_;
			boost::bsignals::scoped_connection signOutConnection_;
			boost::bsignals::scoped_connection uiEventConnection_;
	};
}
