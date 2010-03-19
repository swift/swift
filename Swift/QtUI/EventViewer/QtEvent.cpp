#include "Swift/QtUI/EventViewer/QtEvent.h"

#include "Swiften/Events/MessageEvent.h"

#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {

QtEvent::QtEvent(boost::shared_ptr<Event> event, bool active) : event_(event) {
	active_ = active;
}

QVariant QtEvent::data(int role) {
 	switch (role) {
		case Qt::DisplayRole: return QVariant(text());
		case Qt::TextColorRole: return active_ ? Qt::black : Qt::darkGray;
		case Qt::BackgroundColorRole: return active_ ? Qt::white : Qt::lightGray;
			/*case Qt::ToolTipRole: return isContact() ? toolTipString() : QVariant();
	 	case StatusTextRole: return statusText_;
		case AvatarRole: return avatar_;
		case PresenceIconRole: return getPresenceIcon();*/
	 	default: return QVariant();
	}
}

QString QtEvent::text() {
	boost::shared_ptr<MessageEvent> messageEvent = boost::dynamic_pointer_cast<MessageEvent>(event_);
	return messageEvent ? P2QSTRING(messageEvent->getStanza()->getBody()) :  "Bob: ";
}

}
