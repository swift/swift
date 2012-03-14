/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/EventNotifier.h"

#include <boost/bind.hpp>

#include <Swift/Controllers/Intl.h>
#include <Swiften/Base/format.h>
#include "Swift/Controllers/XMPPEvents/EventController.h"
#include "SwifTools/Notifier/Notifier.h"
#include "Swiften/Avatars/AvatarManager.h"
#include "Swiften/Client/NickResolver.h"
#include "Swiften/JID/JID.h"
#include "Swift/Controllers/XMPPEvents/MessageEvent.h"
#include "Swift/Controllers/XMPPEvents/SubscriptionRequestEvent.h"
#include "Swift/Controllers/XMPPEvents/ErrorEvent.h"
#include "Swift/Controllers/Settings/SettingsProvider.h"

namespace Swift {

EventNotifier::EventNotifier(EventController* eventController, Notifier* notifier, AvatarManager* avatarManager, NickResolver* nickResolver) : eventController(eventController), notifier(notifier), avatarManager(avatarManager), nickResolver(nickResolver) {
	eventController->onEventQueueEventAdded.connect(boost::bind(&EventNotifier::handleEventAdded, this, _1));
}

EventNotifier::~EventNotifier() {
	notifier->purgeCallbacks();
	eventController->onEventQueueEventAdded.disconnect(boost::bind(&EventNotifier::handleEventAdded, this, _1));
}

void EventNotifier::handleEventAdded(boost::shared_ptr<StanzaEvent> event) {
	if (event->getConcluded()) {
		return;
	}
	if (boost::shared_ptr<MessageEvent> messageEvent = boost::dynamic_pointer_cast<MessageEvent>(event)) {
		JID jid = messageEvent->getStanza()->getFrom();
		std::string title = nickResolver->jidToNick(jid);
		if (!messageEvent->getStanza()->isError() && !messageEvent->getStanza()->getBody().empty()) {
			JID activationJID = jid;
			if (messageEvent->getStanza()->getType() == Message::Groupchat) {
				activationJID = jid.toBare();
			}
			notifier->showMessage(Notifier::IncomingMessage, title, messageEvent->getStanza()->getBody(), avatarManager->getAvatarPath(jid), boost::bind(&EventNotifier::handleNotificationActivated, this, activationJID));
		}
	}
	else if(boost::shared_ptr<SubscriptionRequestEvent> subscriptionEvent = boost::dynamic_pointer_cast<SubscriptionRequestEvent>(event)) {
		JID jid = subscriptionEvent->getJID();
		std::string title = jid;
		std::string message = str(format(QT_TRANSLATE_NOOP("", "%1% wants to add you to his/her contact list")) % nickResolver->jidToNick(jid));
		notifier->showMessage(Notifier::SystemMessage, title, message, boost::filesystem::path(), boost::function<void()>());
	}
	else if(boost::shared_ptr<ErrorEvent> errorEvent = boost::dynamic_pointer_cast<ErrorEvent>(event)) {
		notifier->showMessage(Notifier::SystemMessage, QT_TRANSLATE_NOOP("", "Error"), errorEvent->getText(), boost::filesystem::path(), boost::function<void()>());
	}
}


void EventNotifier::handleNotificationActivated(JID jid) {
	onNotificationActivated(jid);
}

}
