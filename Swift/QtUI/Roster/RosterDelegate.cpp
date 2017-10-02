/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/Roster/RosterDelegate.h>

#include <QApplication>
#include <QBitmap>
#include <QBrush>
#include <QColor>
#include <QDebug>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPolygon>

#include <Swift/Controllers/Roster/ContactRosterItem.h>
#include <Swift/Controllers/Roster/GroupRosterItem.h>

#include <Swift/QtUI/Roster/QtTreeWidget.h>
#include <Swift/QtUI/Roster/RosterModel.h>

namespace Swift {

RosterDelegate::RosterDelegate(QtTreeWidget* tree, bool compact) : compact_(compact) {
    tree_ = tree;
    groupDelegate_ = new GroupItemDelegate();
}

RosterDelegate::~RosterDelegate() {
    delete groupDelegate_;
}

void RosterDelegate::setCompact(bool compact) {
    compact_ = compact;
    emit sizeHintChanged(QModelIndex());
}

QSize RosterDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
    RosterItem* item = static_cast<RosterItem*>(index.internalPointer());
    if (dynamic_cast<GroupRosterItem*>(item)) {
        return groupDelegate_->sizeHint(option, index);
    }
    return contactSizeHint(option, index);
}

QSize RosterDelegate::contactSizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
    return common_.contactSizeHint(option, index, compact_);
}

void RosterDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    RosterItem* item = static_cast<RosterItem*>(index.internalPointer());
    if (dynamic_cast<GroupRosterItem*>(item)) {
        paintGroup(painter, option, index);
    } else {
        paintContact(painter, option, index);
    }
}

void RosterDelegate::paintGroup(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    if (index.isValid()) {
        groupDelegate_->paint(painter, option, index.data(Qt::DisplayRole).toString(), index.data(ChildCountRole).toInt(), tree_->isExpanded(index));
    }
}

void RosterDelegate::paintContact(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QColor nameColor = index.data(Qt::TextColorRole).value<QColor>();
    QString avatarPath;
    if (index.data(AvatarRole).isValid() && !index.data(AvatarRole).value<QString>().isNull()) {
        avatarPath = index.data(AvatarRole).value<QString>();
    }
    QIcon presenceIcon = index.data(PresenceIconRole).isValid() && !index.data(PresenceIconRole).value<QIcon>().isNull()
            ? index.data(PresenceIconRole).value<QIcon>()
            : QIcon(":/icons/offline.png");
    bool isIdle = index.data(IdleRole).isValid() ? index.data(IdleRole).toBool() : false;
    QString name = index.data(Qt::DisplayRole).toString();
    QString statusText = index.data(StatusTextRole).toString();
    common_.paintContact(painter, option, nameColor, avatarPath, presenceIcon, name, statusText, isIdle, 0, compact_);
}

}

