/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/ChatList/ChatListWhiteboardItem.h>

#include <QColor>

#include <Swiften/Base/Path.h>

#include <Swift/QtUI/QtResourceHelper.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {
    ChatListWhiteboardItem::ChatListWhiteboardItem(const ChatListWindow::Chat& chat, ChatListGroupItem* parent) : ChatListItem(parent), chat_(chat) {

    }

    const ChatListWindow::Chat& ChatListWhiteboardItem::getChat() const {
        return chat_;
    }

    QVariant ChatListWhiteboardItem::data(int role) const {
        switch (role) {
        case Qt::DisplayRole: return P2QSTRING(chat_.chatName);
        case DetailTextRole: return P2QSTRING(chat_.activity);
        case Qt::TextColorRole: return QColor(89,89,89);
        /*case Qt::TextColorRole: return textColor_;
        case Qt::BackgroundColorRole: return backgroundColor_;
        case Qt::ToolTipRole: return isContact() ? toolTipString() : QVariant();
        case StatusTextRole: return statusText_;*/
        case AvatarRole: return QVariant(P2QSTRING(pathToString(chat_.avatarPath)));
        case PresenceIconRole: return getPresenceIcon();
        default: return QVariant();
        }
    }

    QIcon ChatListWhiteboardItem::getPresenceIcon() const {
        return QIcon(statusShowTypeToIconPath(chat_.statusType));
    }
}

