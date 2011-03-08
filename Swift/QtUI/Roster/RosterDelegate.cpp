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
#include <QFileInfo>
#include <QPolygon>
#include <qdebug.h>
#include <QBitmap>

#include "Swift/Controllers/Roster/ContactRosterItem.h"
#include "Swift/Controllers/Roster/GroupRosterItem.h"

#include "QtTreeWidget.h"
#include "RosterModel.h"
#include "QtScaledAvatarCache.h"

namespace Swift {

RosterDelegate::RosterDelegate(QtTreeWidget* tree) {
	tree_ = tree;
	groupDelegate_ = new GroupItemDelegate();
}

RosterDelegate::~RosterDelegate() {
	delete groupDelegate_;
}
	
QSize RosterDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	RosterItem* item = static_cast<RosterItem*>(index.internalPointer());
	if (dynamic_cast<GroupRosterItem*>(item)) {
		return groupDelegate_->sizeHint(option, index);
	}
	return contactSizeHint(option, index);
}

QSize RosterDelegate::contactSizeHint(const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/ ) const {
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

	QPixmap avatarPixmap;
	if (index.data(AvatarRole).isValid() && !index.data(AvatarRole).value<QString>().isNull()) {
		QString avatarPath = index.data(AvatarRole).value<QString>();
		QString scaledAvatarPath = QtScaledAvatarCache(avatarRegion.height()).getScaledAvatarPath(avatarPath);
		if (QFileInfo(scaledAvatarPath).exists()) {
			avatarPixmap.load(scaledAvatarPath);
		}
	}
	if (avatarPixmap.isNull()) {
		avatarPixmap = QPixmap(":/icons/avatar.png").scaled(avatarRegion.height(), avatarRegion.width(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	painter->drawPixmap(avatarRegion.topLeft() + QPoint(((avatarRegion.width() - avatarPixmap.width()) / 2), (avatarRegion.height() - avatarPixmap.height()) / 2), avatarPixmap);

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
	
	DelegateCommons::drawElidedText(painter, nameRegion, index.data(Qt::DisplayRole).toString());
	
	
	painter->setFont(common_.detailFont);
	painter->setPen(QPen(QColor(160,160,160)));
	
	QRect statusTextRegion(textRegion.adjusted(0, nameHeight, 0, 0));
	DelegateCommons::drawElidedText(painter, statusTextRegion, index.data(StatusTextRole).toString());
	
	painter->restore();
}


const int RosterDelegate::avatarSize_ = 20;
const int RosterDelegate::presenceIconHeight_ = 16;
const int RosterDelegate::presenceIconWidth_ = 16;

}

