/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/QtUI/ChatList/ChatListRecentItem.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {
ChatListRecentItem::ChatListRecentItem(const ChatListWindow::Chat& chat, ChatListGroupItem* parent) : ChatListItem(parent), chat_(chat) {

}

const ChatListWindow::Chat& ChatListRecentItem::getChat() const {
	return chat_;
}

QVariant ChatListRecentItem::data(int role) const {
	switch (role) {
		case Qt::DisplayRole: return P2QSTRING(chat_.chatName);
		case DetailTextRole: return P2QSTRING(chat_.activity);
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
