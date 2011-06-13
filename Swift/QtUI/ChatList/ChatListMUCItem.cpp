/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/ChatList/ChatListMUCItem.h"

#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {
ChatListMUCItem::ChatListMUCItem(const MUCBookmark& bookmark, ChatListGroupItem* parent) : ChatListItem(parent), bookmark_(bookmark) {

}

const MUCBookmark& ChatListMUCItem::getBookmark() const {
	return bookmark_;
}

QVariant ChatListMUCItem::data(int role) const {
	switch (role) {
		case Qt::DisplayRole: return P2QSTRING(bookmark_.getName());
		case DetailTextRole: return P2QSTRING(bookmark_.getRoom().toString());
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
