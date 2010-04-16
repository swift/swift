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

#include "QtTreeWidgetItem.h"

namespace Swift {

RosterDelegate::RosterDelegate() {
	groupDelegate_ = new GroupItemDelegate();
}

RosterDelegate::~RosterDelegate() {
	delete groupDelegate_;
}
	
QSize RosterDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	QtTreeWidgetItem* item = static_cast<QtTreeWidgetItem*>(index.internalPointer());
	if (!item || !item->isContact()) {
		return groupDelegate_->sizeHint(option, index);
	}
	return contactSizeHint(option, index);
}

QSize RosterDelegate::contactSizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	Q_UNUSED(option);
	Q_UNUSED(index);
	int heightByAvatar = avatarSize_ + common_.verticalMargin * 2;
	QFontMetrics nameMetrics(common_.nameFont);
	QFontMetrics statusMetrics(common_.detailFont);
	int sizeByText = 2 * common_.verticalMargin + nameMetrics.height() + statusMetrics.height();
	//Doesn't work, yay! FIXME: why?
	//QSize size = (option.state & QStyle::State_Selected) ? QSize(150, 80) : QSize(150, avatarSize_ + margin_ * 2);
	//qDebug() << "Returning size" << size;
	return QSize(150, sizeByText > heightByAvatar ? sizeByText : heightByAvatar);
}

void RosterDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	QtTreeWidgetItem* item = static_cast<QtTreeWidgetItem*>(index.internalPointer());
	if (item && !item->isContact()) {
		paintGroup(painter, option, index);
	} else {
		paintContact(painter, option, index);
	}
}

void RosterDelegate::paintGroup(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	QtTreeWidgetItem* item = index.isValid() ? static_cast<QtTreeWidgetItem*>(index.internalPointer()) : NULL;
	if (item) {
		groupDelegate_->paint(painter, option, index.data(Qt::DisplayRole).toString(), item->rowCount(), item->isExpanded());
	}
}




void RosterDelegate::paintContact(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	//qDebug() << "painting" << index.data(Qt::DisplayRole).toString();
	painter->save();
	//QStyledItemDelegate::paint(painter, option, index);
	//initStyleOption(option, index);
	QRect fullRegion(option.rect);
	if ( option.state & QStyle::State_Selected ) {
		painter->fillRect(fullRegion, option.palette.highlight());
		painter->setPen(option.palette.highlightedText().color());
	} else {
		QColor nameColor = index.data(Qt::TextColorRole).value<QColor>();
		painter->setPen(QPen(nameColor));
	}
	
	QRect presenceIconRegion(QPoint(common_.farLeftMargin, fullRegion.top()), QSize(presenceIconWidth_, fullRegion.height() - common_.verticalMargin));
	
	int calculatedAvatarSize = presenceIconRegion.height();
	//This overlaps the presenceIcon, so must be painted first
	QRect avatarRegion(QPoint(presenceIconRegion.right() - presenceIconWidth_ / 2, presenceIconRegion.top()), QSize(calculatedAvatarSize, calculatedAvatarSize));
	QIcon avatar = index.data(AvatarRole).isValid() && !index.data(AvatarRole).value<QIcon>().isNull()
		? index.data(AvatarRole).value<QIcon>()
		: QIcon(":/icons/avatar.png");
	avatar.paint(painter, avatarRegion, Qt::AlignVCenter | Qt::AlignHCenter);

	//Paint the presence icon over the top of the avatar
	QIcon presenceIcon = index.data(PresenceIconRole).isValid() && !index.data(PresenceIconRole).value<QIcon>().isNull()
		? index.data(PresenceIconRole).value<QIcon>()
		: QIcon(":/icons/offline.png");
	presenceIcon.paint(painter, presenceIconRegion, Qt::AlignBottom | Qt::AlignHCenter);
	
	QFontMetrics nameMetrics(common_.nameFont);
	painter->setFont(common_.nameFont);
	int extraFontWidth = nameMetrics.width("H");
	int leftOffset = avatarRegion.right() + common_.horizontalMargin * 2 + extraFontWidth / 2;
	QRect textRegion(fullRegion.adjusted(leftOffset, 0, 0/*-leftOffset*/, 0));
	
	int nameHeight = nameMetrics.height() + common_.verticalMargin;
	QRect nameRegion(textRegion.adjusted(0, common_.verticalMargin, 0, 0));
	
	painter->drawText(nameRegion, Qt::AlignTop, index.data(Qt::DisplayRole).toString());
	
	
	painter->setFont(common_.detailFont);
	painter->setPen(QPen(QColor(160,160,160)));
	
	QRect statusTextRegion(textRegion.adjusted(0, nameHeight, 0, 0));
	painter->drawText(statusTextRegion, Qt::AlignTop, index.data(StatusTextRole).toString());
	
	painter->restore();
}

const int RosterDelegate::avatarSize_ = 20;
const int RosterDelegate::presenceIconHeight_ = 16;
const int RosterDelegate::presenceIconWidth_ = 16;

}
