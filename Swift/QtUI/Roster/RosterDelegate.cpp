/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "RosterDelegate.h"

#include <QApplication>
#include <QPainter>
#include <QColor>
#include <QBrush>
#include <QFontMetrics>
#include <QPainterPath>
#include <QPolygon>
#include <qdebug.h>
#include <QBitmap>

#include "Swift/Controllers/Roster/ContactRosterItem.h"
#include "Swift/Controllers/Roster/GroupRosterItem.h"

#include "QtTreeWidget.h"
#include "RosterModel.h"

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
	QString name = index.data(Qt::DisplayRole).toString();
	QString statusText = index.data(StatusTextRole).toString();
	common_.paintContact(painter, option, nameColor, avatarPath, presenceIcon, name, statusText, 0, compact_);
}

}

