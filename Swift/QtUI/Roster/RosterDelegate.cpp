#include "RosterDelegate.h"

#include <QApplication>
#include <QPainter>
#include <QColor>
#include <QBrush>
#include <QFontMetrics>
#include <QPainterPath>
#include <qdebug.h>

#include "QtTreeWidgetItem.h"

namespace Swift {

RosterDelegate::RosterDelegate() : nameFont_(QApplication::font()), statusFont_(QApplication::font()) {
	int statusFontSizeDrop = nameFont_.pointSize() >= 10 ? 2 : 0;
	statusFont_.setStyle(QFont::StyleItalic);
	statusFont_.setPointSize(nameFont_.pointSize() - statusFontSizeDrop);
}
	
QSize RosterDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	QtTreeWidgetItem* item = static_cast<QtTreeWidgetItem*>(index.internalPointer());
	if (!item || !item->isContact()) {
		return QStyledItemDelegate::sizeHint(option, index);
	}
	int sizeByAvatar = avatarSize_ + margin_ * 2;
	QFontMetrics nameMetrics(nameFont_);
	QFontMetrics statusMetrics(statusFont_);
	int sizeByText = 2 * margin_ + nameMetrics.height() + statusMetrics.height();
	//Doesn't work, yay! FIXME: why?
	//QSize size = (option.state & QStyle::State_Selected) ? QSize(150, 80) : QSize(150, avatarSize_ + margin_ * 2);
	//qDebug() << "Returning size" << size;
	return QSize(150, sizeByText > sizeByAvatar ? sizeByText : sizeByAvatar);
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
	painter->save();		
	QLinearGradient fillGradient(option.rect.topLeft(), option.rect.bottomLeft());
	fillGradient.setColorAt(0, QColor(200, 200, 200));
	fillGradient.setColorAt(0.5, QColor(150, 150, 150));
	fillGradient.setColorAt(1, QColor(200, 200, 200));
	QBrush backgroundBrush = QBrush(fillGradient);
	painter->setPen(QPen(index.data(Qt::TextColorRole).value<QColor>()));
	QPainterPath roundedPath;
	roundedPath.addRoundedRect(option.rect, 5, 5);
	painter->fillPath(roundedPath, backgroundBrush);
	painter->drawText(option.rect.adjusted(margin_, 0, -1 * margin_, 0), Qt::AlignTop, index.data(Qt::DisplayRole).toString());
	painter->restore();
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
	
	QRect presenceIconRegion(QPoint(margin_, fullRegion.top()), QSize(presenceIconWidth_, fullRegion.height()));
	
	//This overlaps the presenceIcon, so must be painted first
	QRect avatarRegion(QPoint(presenceIconRegion.right() - presenceIconWidth_ / 2, fullRegion.top()), QSize(avatarSize_, fullRegion.height()));
	QIcon avatar = index.data(AvatarRole).isValid() && !index.data(AvatarRole).value<QIcon>().isNull()
		? index.data(AvatarRole).value<QIcon>()
		: QIcon(":/icons/avatar.png");
	avatar.paint(painter, avatarRegion, Qt::AlignVCenter | Qt::AlignHCenter);

	//Paint the presence icon over the top of the avatar
	QIcon presenceIcon = index.data(PresenceIconRole).isValid() && !index.data(PresenceIconRole).value<QIcon>().isNull()
		? index.data(PresenceIconRole).value<QIcon>()
		: QIcon(":/icons/offline.png");
	presenceIcon.paint(painter, presenceIconRegion, Qt::AlignBottom | Qt::AlignHCenter);
	
	painter->setFont(nameFont_);
	QRect textRegion(fullRegion.adjusted(avatarRegion.right() + margin_ * 2, 0, 0, 0));
	
	QFontMetrics nameMetrics(nameFont_);
	int nameHeight = nameMetrics.height() + margin_;
	QRect nameRegion(textRegion.adjusted(0, margin_, 0, 0));
	
	painter->drawText(nameRegion, Qt::AlignTop, index.data(Qt::DisplayRole).toString());
	
	
	painter->setFont(statusFont_);
	painter->setPen(QPen(QColor(160,160,160)));
	QRect statusTextRegion(textRegion.adjusted(0, nameHeight, 0, 0));
	painter->drawText(statusTextRegion, Qt::AlignTop, index.data(StatusTextRole).toString());
	
	painter->restore();
}

}