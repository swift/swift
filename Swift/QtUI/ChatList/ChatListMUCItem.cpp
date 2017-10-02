/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/ChatList/ChatListMUCItem.h>

#include <QColor>

#include <Swift/QtUI/QtSwiftUtil.h>

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
        case Qt::TextColorRole: return QColor(89,89,89);
        /*case Qt::BackgroundColorRole: return backgroundColor_;
        case Qt::ToolTipRole: return isContact() ? toolTipString() : QVariant();
        case StatusTextRole: return statusText_;
        case AvatarRole: return avatar_;
        case PresenceIconRole: return getPresenceIcon();*/
        default: return QVariant();
    }
}

}
