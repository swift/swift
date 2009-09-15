#include "Swift/Controllers/RosterController.h"

#include <boost/bind.hpp>

#include "Swiften/Base/foreach.h"
#include "Swift/Controllers/MainWindow.h"
#include "Swift/Controllers/MainWindowFactory.h"
#include "Swift/Controllers/NickResolver.h"
#include "Swiften/Queries/Requests/GetRosterRequest.h"
#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/Roster/Roster.h"
#include "Swiften/Roster/SetPresence.h"
#include "Swiften/Roster/AppearOffline.h"
#include "Swiften/Roster/SetAvatar.h"
#include "Swiften/Roster/OfflineRosterFilter.h"
#include "Swiften/Roster/OpenChatRosterAction.h"
#include "Swiften/Roster/TreeWidgetFactory.h"
#include "Swiften/Roster/XMPPRoster.h"


namespace Swift {
	
/**
 * The controller does not gain ownership of these parameters.
 */
RosterController::RosterController(const JID& jid, boost::shared_ptr<XMPPRoster> xmppRoster, AvatarManager* avatarManager, MainWindowFactory* mainWindowFactory, TreeWidgetFactory* treeWidgetFactory, NickResolver* nickResolver)
 : myJID_(jid), xmppRoster_(xmppRoster), mainWindowFactory_(mainWindowFactory), treeWidgetFactory_(treeWidgetFactory), mainWindow_(mainWindowFactory_->createMainWindow()), roster_(new Roster(mainWindow_->getTreeWidget(), treeWidgetFactory_)), offlineFilter_(new OfflineRosterFilter()) {
	
	roster_->addFilter(offlineFilter_);
	
	mainWindow_->onStartChatRequest.connect(boost::bind(&RosterController::handleStartChatRequest, this, _1));
	mainWindow_->onJoinMUCRequest.connect(boost::bind(&RosterController::handleJoinMUCRequest, this, _1, _2));
	mainWindow_->onChangeStatusRequest.connect(boost::bind(&RosterController::handleChangeStatusRequest, this, _1, _2));
	mainWindow_->onShowOfflineToggled.connect(boost::bind(&RosterController::handleShowOfflineToggled, this, _1));
	roster_->onUserAction.connect(boost::bind(&RosterController::handleUserAction, this, _1));
	xmppRoster_->onJIDAdded.connect(boost::bind(&RosterController::handleOnJIDAdded, this, _1));
	avatarManager_ = NULL;
	setAvatarManager(avatarManager);
	setNickResolver(nickResolver);
	
}

RosterController::~RosterController() {
	delete offlineFilter_;

}

void RosterController::setNickResolver(NickResolver* nickResolver) {
	nickResolver_ = nickResolver;
	if (nickResolver_ != NULL) {
		mainWindow_->setMyName(nickResolver_->jidToNick(myJID_));
	}
}

void RosterController::setAvatarManager(AvatarManager* avatarManager) {
	if (avatarManager_ != NULL) {
		//FIXME: disconnect old signal;
	}
	avatarManager_ = avatarManager;
	if (avatarManager != NULL) {
		avatarManager_->onAvatarChanged.connect(boost::bind(&RosterController::handleAvatarChanged, this, _1, _2));
		mainWindow_->setMyAvatarPath(avatarManager_->getAvatarPath(myJID_).string());
	}
}

void RosterController::setEnabled(bool enabled) {
	if (!enabled) {
		roster_->applyOnItems(AppearOffline());
	}
}

void RosterController::handleShowOfflineToggled(bool state) {
	if (state) {
		roster_->removeFilter(offlineFilter_);
	} else {
		roster_->addFilter(offlineFilter_);
	}
}

void RosterController::handleChangeStatusRequest(StatusShow::Type show, const String &statusText) {
	onChangeStatusRequest(show, statusText);
}

void RosterController::handleUserAction(boost::shared_ptr<UserRosterAction> action) {
	boost::shared_ptr<OpenChatRosterAction> chatAction = boost::dynamic_pointer_cast<OpenChatRosterAction>(action);
	if (chatAction.get() != NULL) {
		ContactRosterItem *contactItem = dynamic_cast<ContactRosterItem*>(chatAction->getRosterItem());
		assert(contactItem);
		onStartChatRequest(contactItem->getJID().toBare());
	}
}

void RosterController::handleOnJIDAdded(const JID& jid) {
	std::vector<String> groups = xmppRoster_->getGroupsForJID(jid);
	String name = xmppRoster_->getNameForJID(jid);
	if (!groups.empty()) {
		foreach(const String& group, groups) {
			roster_->addContact(jid, name, group);
		}
	} else {
		roster_->addContact(jid, name, "Contacts");
	}
}

void RosterController::handleIncomingPresence(boost::shared_ptr<Presence> presence) {
	roster_->applyOnItems(SetPresence(presence));
}

void RosterController::handleAvatarChanged(const JID& jid, const String& hash) {
	String path = avatarManager_->getAvatarPath(jid).string();
	roster_->applyOnItems(SetAvatar(jid, path));
	if (jid.equals(myJID_, JID::WithoutResource)) {
		mainWindow_->setMyAvatarPath(path);
	}
}

void RosterController::handleStartChatRequest(const JID& contact) {
	onStartChatRequest(contact);
}

void RosterController::handleJoinMUCRequest(const JID &muc, const String &nick) {
	onJoinMUCRequest(JID(muc), nick);
}

}
