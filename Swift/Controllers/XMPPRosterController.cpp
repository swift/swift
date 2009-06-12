#include "Swift/Controllers/XMPPRosterController.h"

#include <boost/bind.hpp>

#include "Swiften/Base/foreach.h"
#include "Swift/Controllers/MainWindow.h"
#include "Swift/Controllers/MainWindowFactory.h"
#include "Swiften/Elements/RosterItemPayload.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Queries/Requests/GetRosterRequest.h"
#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/Roster/Roster.h"
#include "Swiften/Roster/SetPresence.h"
#include "Swiften/Roster/OfflineRosterFilter.h"
#include "Swiften/Roster/OpenChatRosterAction.h"
#include "Swiften/Roster/TreeWidgetFactory.h"
#include "Swiften/Roster/XMPPRoster.h"

namespace Swift {
	
/**
 * The controller does not gain ownership of these parameters.
 */
XMPPRosterController::XMPPRosterController(IQRouter* iqRouter, boost::shared_ptr<XMPPRoster> xmppRoster) : iqRouter_(iqRouter), rosterPushResponder_(iqRouter), xmppRoster_(xmppRoster) {
	rosterPushResponder_.onRosterReceived.connect(boost::bind(&XMPPRosterController::handleRosterReceived, this, _1));
}

void XMPPRosterController::requestRoster() {
	GetRosterRequest* rosterRequest = new GetRosterRequest(iqRouter_, Request::AutoDeleteAfterResponse);
	rosterRequest->onResponse.connect(boost::bind(&XMPPRosterController::handleRosterReceived, this, _1));
	rosterRequest->send();
}

void XMPPRosterController::handleRosterReceived(boost::shared_ptr<RosterPayload> rosterPayload) {
	foreach(const RosterItemPayload& item, rosterPayload->getItems()) {
		if (item.getSubscription() == RosterItemPayload::Remove) {
			xmppRoster_->removeContact(item.getJID());
		} else {
			xmppRoster_->addContact(item.getJID(), item.getName(), item.getGroups());
		}
	}
}

}
