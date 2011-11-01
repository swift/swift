/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QPen>
#include <QPainter>

#include "Swift/QtUI/ChatList/ChatListDelegate.h"
#include "Swift/QtUI/Roster/GroupItemDelegate.h"
#include "Swift/QtUI/ChatList/ChatListItem.h"
#include "Swift/QtUI/ChatList/ChatListMUCItem.h"
#include "Swift/QtUI/ChatList/ChatListRecentItem.h"
#include "Swift/QtUI/ChatList/ChatListGroupItem.h"

namespace Swift {

ChatListDelegate::ChatListDelegate(bool compact) : compact_(compact) {
	groupDelegate_ = new GroupItemDelegate();
}

ChatListDelegate::~ChatListDelegate() {
	delete groupDelegate_;
}

void ChatListDelegate::setCompact(bool compact) {
	compact_ = compact;
}

QSize ChatListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	ChatListItem* item = static_cast<ChatListItem*>(index.internalPointer());
	if (item && dynamic_cast<ChatListMUCItem*>(item)) {
		return mucSizeHint(option, index);
	}
	else if (item && dynamic_cast<ChatListRecentItem*>(item)) {
		return common_.contactSizeHint(option, index, compact_);
	}
	else if (item && dynamic_cast<ChatListGroupItem*>(item)) {
		return groupDelegate_->sizeHint(option, index);
	} 
	return QStyledItemDelegate::sizeHint(option, index);
}

QSize ChatListDelegate::mucSizeHint(const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/ ) const {
	QFontMetrics nameMetrics(common_.nameFont);
	QFontMetrics statusMetrics(common_.detailFont);
	int sizeByText = 2 * common_.verticalMargin + nameMetrics.height() + statusMetrics.height();
	return QSize(150, sizeByText);
}

QSize ChatListDelegate::recentSizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
	return mucSizeHint(option, index);
}

void ChatListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	ChatListItem* item = static_cast<ChatListItem*>(index.internalPointer());
	if (item && dynamic_cast<ChatListMUCItem*>(item)) {
		paintMUC(painter, option, dynamic_cast<ChatListMUCItem*>(item));
	}
	else if (item && dynamic_cast<ChatListRecentItem*>(item)) {
		paintRecent(painter, option, dynamic_cast<ChatListRecentItem*>(item));
	}
	else if (item && dynamic_cast<ChatListGroupItem*>(item)) {
		ChatListGroupItem* group = dynamic_cast<ChatListGroupItem*>(item);
		groupDelegate_->paint(painter, option, group->data(Qt::DisplayRole).toString(), group->rowCount(), option.state & QStyle::State_Open); 
	}
	else {
		QStyledItemDelegate::paint(painter, option, index);
	}
}

void ChatListDelegate::paintMUC(QPainter* painter, const QStyleOptionViewItem& option, ChatListMUCItem* item) const {
	painter->save();
	QRect fullRegion(option.rect);
	if ( option.state & QStyle::State_Selected ) {
		painter->fillRect(fullRegion, option.palette.highlight());
		painter->setPen(option.palette.highlightedText().color());
	} else {
		QColor nameColor = item->data(Qt::TextColorRole).value<QColor>();
		painter->setPen(QPen(nameColor));
	}

	QFontMetrics nameMetrics(common_.nameFont);
	painter->setFont(common_.nameFont);
	int extraFontWidth = nameMetrics.width("H");
	int leftOffset = common_.horizontalMargin * 2 + extraFontWidth / 2;
	QRect textRegion(fullRegion.adjusted(leftOffset, 0, 0, 0));
	
	int nameHeight = nameMetrics.height() + common_.verticalMargin;
	QRect nameRegion(textRegion.adjusted(0, common_.verticalMargin, 0, 0));
	
	DelegateCommons::drawElidedText(painter, nameRegion, item->data(Qt::DisplayRole).toString());
	
	painter->setFont(common_.detailFont);
	painter->setPen(QPen(QColor(160,160,160)));
	
	QRect detailRegion(textRegion.adjusted(0, nameHeight, 0, 0));
	DelegateCommons::drawElidedText(painter, detailRegion, item->data(ChatListMUCItem::DetailTextRole).toString());
	
	painter->restore();
}

void ChatListDelegate::paintRecent(QPainter* painter, const QStyleOptionViewItem& option, ChatListRecentItem* item) const {
	QColor nameColor = item->data(Qt::TextColorRole).value<QColor>();
	QString avatarPath;
	if (item->data(ChatListRecentItem::AvatarRole).isValid() && !item->data(ChatListRecentItem::AvatarRole).value<QString>().isNull()) {
		avatarPath = item->data(ChatListRecentItem::AvatarRole).value<QString>();
	}
	QIcon presenceIcon = item->data(ChatListRecentItem::PresenceIconRole).isValid() && !item->data(ChatListRecentItem::PresenceIconRole).value<QIcon>().isNull()
			? item->data(ChatListRecentItem::PresenceIconRole).value<QIcon>()
			: QIcon(":/icons/offline.png");
	QString name = item->data(Qt::DisplayRole).toString();
	//qDebug() << "Avatar for " << name << " = " << avatarPath;
	QString statusText = item->data(ChatListRecentItem::DetailTextRole).toString();
	common_.paintContact(painter, option, nameColor, avatarPath, presenceIcon, name, statusText, item->getChat().unreadCount, compact_);
}

}
