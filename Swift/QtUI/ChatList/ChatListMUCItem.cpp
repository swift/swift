#include "Swift/QtUI/ChatList/ChatListMUCItem.h"

#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {
ChatListMUCItem::ChatListMUCItem(boost::shared_ptr<MUCBookmark> bookmark, ChatListGroupItem* parent) : ChatListItem(parent), bookmark_(bookmark) {

}

boost::shared_ptr<MUCBookmark> ChatListMUCItem::getBookmark() {
	return bookmark_;
}

QVariant ChatListMUCItem::data(int role) { 
	printf("Getting role %d\n", role);
	switch (role) {
		case Qt::DisplayRole: return P2QSTRING(bookmark_->getName());
			/*case Qt::TextColorRole: return textColor_;
		case Qt::BackgroundColorRole: return backgroundColor_;
		case Qt::ToolTipRole: return isContact() ? toolTipString() : QVariant();
		case StatusTextRole: return statusText_;
		case AvatarRole: return avatar_;
		case PresenceIconRole: return getPresenceIcon();*/
		default: return QVariant();
	}
}

}
