/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/EventNotifier.h>

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>

#include <Swiften/Avatars/AvatarManager.h>
#include <Swiften/Base/String.h>
#include <Swiften/Base/format.h>
#include <Swiften/Client/NickResolver.h>
#include <Swiften/JID/JID.h>

#include <Swift/Controllers/Intl.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swift/Controllers/XMPPEvents/ErrorEvent.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>
#include <Swift/Controllers/XMPPEvents/MUCInviteEvent.h>
#include <Swift/Controllers/XMPPEvents/MessageEvent.h>
#include <Swift/Controllers/XMPPEvents/SubscriptionRequestEvent.h>

#include <SwifTools/Notifier/Notifier.h>

namespace Swift {

EventNotifier::EventNotifier(EventController* eventController, Notifier* notifier, AvatarManager* avatarManager, NickResolver* nickResolver) : eventController(eventController), notifier(notifier), avatarManager(avatarManager), nickResolver(nickResolver) {
    eventController->onEventQueueEventAdded.connect(boost::bind(&EventNotifier::handleEventAdded, this, _1));
}

EventNotifier::~EventNotifier() {
    notifier->purgeCallbacks();
    eventController->onEventQueueEventAdded.disconnect(boost::bind(&EventNotifier::handleEventAdded, this, _1));
}

void EventNotifier::handleEventAdded(std::shared_ptr<StanzaEvent> event) {
    if (event->getConcluded()) {
        return;
    }
    if (std::shared_ptr<MessageEvent> messageEvent = std::dynamic_pointer_cast<MessageEvent>(event)) {
        JID jid = messageEvent->getStanza()->getFrom();
        if (!messageEvent->getStanza()->isError() && !messageEvent->getStanza()->getBody().get_value_or("").empty()) {
            JID activationJID = jid;
            if (messageEvent->getStanza()->getType() == Message::Groupchat) {
                activationJID = jid.toBare();
            }
            for (const auto& notification : messageEvent->getNotifications()) {
                notifier->showMessage(Notifier::IncomingMessage, notification.title, notification.message, avatarManager->getAvatarPath(jid), boost::bind(&EventNotifier::handleNotificationActivated, this, activationJID));
            }
        }
    }
    else if(std::shared_ptr<SubscriptionRequestEvent> subscriptionEvent = std::dynamic_pointer_cast<SubscriptionRequestEvent>(event)) {
        JID jid = subscriptionEvent->getJID();
        std::string title = jid;
        std::string message = str(format(QT_TRANSLATE_NOOP("", "%1% wants to add you to his/her contact list")) % nickResolver->jidToNick(jid));
        notifier->showMessage(Notifier::SystemMessage, title, message, boost::filesystem::path(), boost::function<void()>());
    }
    else if(std::shared_ptr<ErrorEvent> errorEvent = std::dynamic_pointer_cast<ErrorEvent>(event)) {
        notifier->showMessage(Notifier::SystemMessage, QT_TRANSLATE_NOOP("", "Error"), errorEvent->getText(), boost::filesystem::path(), boost::function<void()>());
    }
    else if (std::shared_ptr<MUCInviteEvent> mucInviteEvent = std::dynamic_pointer_cast<MUCInviteEvent>(event)) {
        std::string title = mucInviteEvent->getInviter();
        std::string message = str(format(QT_TRANSLATE_NOOP("", "%1% has invited you to enter the %2% room")) % nickResolver->jidToNick(mucInviteEvent->getInviter()) % mucInviteEvent->getRoomJID());
        // FIXME: not show avatar or greyed out avatar for mediated invites
        notifier->showMessage(Notifier::SystemMessage, title, message, avatarManager->getAvatarPath(mucInviteEvent->getInviter()), boost::bind(&EventNotifier::handleNotificationActivated, this, mucInviteEvent->getInviter()));
    }
}


void EventNotifier::handleNotificationActivated(JID jid) {
    onNotificationActivated(jid);
}

}
