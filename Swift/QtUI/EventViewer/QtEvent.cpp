#include "Swift/QtUI/EventViewer/QtEvent.h"

#include "Swiften/Events/MessageEvent.h"
#include "Swiften/Events/SubscriptionRequestEvent.h"

#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {

QtEvent::QtEvent(boost::shared_ptr<StanzaEvent> event, bool active) : event_(event) {
	active_ = active;
}

QVariant QtEvent::data(int role) {
 	switch (role) {
		case Qt::ToolTipRole:
		case Qt::DisplayRole: return QVariant(text());
		case Qt::TextColorRole: return active_ ? Qt::black : Qt::darkGray;
		case Qt::BackgroundColorRole: return active_ ? Qt::white : Qt::lightGray;
			/*case StatusTextRole: return statusText_;
		case AvatarRole: return avatar_;
		case PresenceIconRole: return getPresenceIcon();*/
	 	default: return QVariant();
	}
}

QString QtEvent::text() {
	boost::shared_ptr<MessageEvent> messageEvent = boost::dynamic_pointer_cast<MessageEvent>(event_);
	if (messageEvent) {
		return P2QSTRING(messageEvent->getStanza()->getBody());
	}
	boost::shared_ptr<SubscriptionRequestEvent> subscriptionRequestEvent = boost::dynamic_pointer_cast<SubscriptionRequestEvent>(event_);
	if (subscriptionRequestEvent) {
		String reason = subscriptionRequestEvent->getReason();
		String message = subscriptionRequestEvent->getJID().toBare().toString() + " would like to add you to their roster" + (reason.isEmpty() ? "." : ", saying '" + reason + "'.");
		return P2QSTRING(message);
	}
	return "";
}

}
