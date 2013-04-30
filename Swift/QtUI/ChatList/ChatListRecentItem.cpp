/*
 * Copyright (c) 2011-2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/QtUI/ChatList/ChatListRecentItem.h>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swiften/Base/Path.h>

namespace Swift {
ChatListRecentItem::ChatListRecentItem(const ChatListWindow::Chat& chat, ChatListGroupItem* parent) : ChatListItem(parent), chat_(chat) {

}

const ChatListWindow::Chat& ChatListRecentItem::getChat() const {
	return chat_;
}

QVariant ChatListRecentItem::data(int role) const {
	switch (role) {
		case Qt::DisplayRole: return chat_.impromptuJIDs.empty() ? P2QSTRING(chat_.chatName) : P2QSTRING(chat_.getImpromptuTitle());
		case DetailTextRole: return P2QSTRING(chat_.activity);
			/*case Qt::TextColorRole: return textColor_;
		case Qt::BackgroundColorRole: return backgroundColor_;
		case Qt::ToolTipRole: return isContact() ? toolTipString() : QVariant();
		case StatusTextRole: return statusText_;*/
		case AvatarRole: return QVariant(P2QSTRING(pathToString(chat_.avatarPath)));
		case PresenceIconRole: return getPresenceIcon();
		default: return QVariant();
	}
}

QIcon ChatListRecentItem::getPresenceIcon() const {
	QString iconString;
	switch (chat_.statusType) {
	 	case StatusShow::Online: iconString = "online";break;
	 	case StatusShow::Away: iconString = "away";break;
	 	case StatusShow::XA: iconString = "away";break;
	 	case StatusShow::FFC: iconString = "online";break;
	 	case StatusShow::DND: iconString = "dnd";break;
	 	case StatusShow::None: iconString = "offline";break;
	}
	return QIcon(":/icons/" + iconString + ".png");
}

}
