/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/ChatList/ChatListRecentItem.h>

#include <Swiften/Base/Path.h>

#include <Swift/QtUI/QtResourceHelper.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {
ChatListRecentItem::ChatListRecentItem(const ChatListWindow::Chat& chat, ChatListGroupItem* parent) : ChatListItem(parent), chat_(chat) {

}

const ChatListWindow::Chat& ChatListRecentItem::getChat() const {
    return chat_;
}

QVariant ChatListRecentItem::data(int role) const {
    switch (role) {
        case Qt::DisplayRole: return P2QSTRING(chat_.getTitle());
        case DetailTextRole: return P2QSTRING(chat_.activity);
        case Qt::TextColorRole: return QColor(89,89,89);
        /*case Qt::BackgroundColorRole: return backgroundColor_;
        case Qt::ToolTipRole: return isContact() ? toolTipString() : QVariant();
        case StatusTextRole: return statusText_;*/
        case AvatarRole: return QVariant(P2QSTRING(pathToString(chat_.avatarPath)));
        case PresenceIconRole: return getPresenceIcon();
        default: return QVariant();
    }
}

QIcon ChatListRecentItem::getPresenceIcon() const {
    return QIcon(statusShowTypeToIconPath(chat_.statusType));
}

}
