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

RosterDelegate::RosterDelegate() : nameFont_(QApplication::font()), statusFont_(QApplication::font()), groupFont_(QApplication::font()) {
	int statusFontSizeDrop = nameFont_.pointSize() >= 10 ? 2 : 0;
	statusFont_.setStyle(QFont::StyleItalic);
	statusFont_.setPointSize(nameFont_.pointSize() - statusFontSizeDrop);
	groupFont_.setPointSize(nameFont_.pointSize() - statusFontSizeDrop);
	groupFont_.setWeight(QFont::Bold);
}
	
QSize RosterDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	QtTreeWidgetItem* item = static_cast<QtTreeWidgetItem*>(index.internalPointer());
	if (!item || !item->isContact()) {
		return groupSizeHint(option, index);
	}
	return contactSizeHint(option, index);
}

QSize RosterDelegate::groupSizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	Q_UNUSED(option);
	Q_UNUSED(index);
	QFontMetrics groupMetrics(groupFont_);
	return QSize(150, groupMetrics.height() + 4);
}

QSize RosterDelegate::contactSizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	Q_UNUSED(option);
	Q_UNUSED(index);
	int heightByAvatar = avatarSize_ + verticalMargin_ * 2;
	QFontMetrics nameMetrics(nameFont_);
	QFontMetrics statusMetrics(statusFont_);
	int sizeByText = 2 * verticalMargin_ + nameMetrics.height() + statusMetrics.height();
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
	painter->save();		
	painter->setPen(QPen(QColor(189, 189, 189)));
	//FIXME: It looks like Qt is passing us a rectangle that's too small
	//This deliberately draws outside the lines, and we need to find a better solution.
	int correctionAmount = groupCornerRadius_ > 0 ? 0 : 1;
	QRect region(QPoint(option.rect.left() - correctionAmount, option.rect.top()), QSize(option.rect.width() + correctionAmount, option.rect.height() - verticalMargin_));
	QLinearGradient fillGradient(region.topLeft(), region.bottomLeft());
	fillGradient.setColorAt(0, QColor(244, 244, 244));
	fillGradient.setColorAt(0.1, QColor(231, 231, 231));
	fillGradient.setColorAt(1, QColor(209, 209, 209));
	
	QBrush backgroundBrush = QBrush(fillGradient);
	QPainterPath fillPath;
	fillPath.addRoundedRect(region, groupCornerRadius_, groupCornerRadius_);
	QPainterPath linePath;
	linePath.addRoundedRect(region, groupCornerRadius_, groupCornerRadius_);
	painter->fillPath(fillPath, backgroundBrush);
	painter->drawPath(linePath);
	
	double triangleWidth = 9;
	double triangleHeight = 5;
	QtTreeWidgetItem* item = index.isValid() ? static_cast<QtTreeWidgetItem*>(index.internalPointer()) : NULL;
	if (item) {
		paintExpansionTriangle(painter, region.adjusted(horizontalMargin_ +  1, 0, 0, 0), triangleWidth, triangleHeight, item->isExpanded());
	}
	
	int textLeftOffset = 3 * horizontalMargin_ + 1 + triangleWidth;
	QFontMetrics fontMetrics(groupFont_);
	int textTopOffset = (option.rect.height() - fontMetrics.height()) / 2;
	painter->setFont(groupFont_);
	int contactCountWidth = 0;
	QRect textRect = region.adjusted(textLeftOffset, textTopOffset, -1 * textLeftOffset, -1 * textTopOffset);

	if (!item->isExpanded()) {
		QFontMetrics groupMetrics(groupFont_);
		int contactCount = item->rowCount();
		QString countString = QString("%1").arg(contactCount);
		contactCountWidth = groupMetrics.width(countString) + 2 * horizontalMargin_;
		int offsetAmount = textRect.width() - contactCountWidth + horizontalMargin_;
		QRect countRect = textRect.adjusted(offsetAmount, 0, 0/*-1 * offsetAmount*/, 0);
		//qDebug() << "Painting count string " << countString << " at " << countRect << " from offset " << offsetAmount;
		paintShadowText(painter, countRect, countString);
	}
	QRect nameTextRect = textRect.adjusted(0, 0, contactCountWidth, 0);
	paintShadowText(painter, nameTextRect, index.data(Qt::DisplayRole).toString());
	painter->restore();
}

void RosterDelegate::paintExpansionTriangle(QPainter* painter, const QRect& region, int width, int height, bool expanded) const {
	QBrush triangleBrush(QColor(110, 110, 110));
	QBrush triangleShadowBrush(QColor(47, 47, 47));
	QPainterPath trianglePath;
	QPainterPath triangleShadowPath;
	QPolygonF triangle;
	QPolygonF triangleShadow;
	QPointF triangleTopLeft(region.left(), region.top() + region.height() / 2 - height / 2);
	QPointF shadowOffset(0,-1);
	QPointF trianglePoint2;
	QPointF trianglePoint3;
	
	if (expanded) {
		triangleTopLeft += QPoint(0, 1);
		trianglePoint2 = triangleTopLeft + QPointF(width, 0);
		trianglePoint3 = trianglePoint2 + QPointF(-1 * (width / 2), height);
		//qDebug() << "Plotting expanded" << triangleTopLeft << ", " << trianglePoint2 << ", " << trianglePoint3;
	} else {
		trianglePoint2 = triangleTopLeft + QPointF(0, width);
		trianglePoint3 = trianglePoint2 + QPointF(height, -1 * (width / 2));
		//qDebug() << "Plotting collapsed" << triangleTopLeft << ", " << trianglePoint2 << ", " << trianglePoint3;
	}
	triangle << triangleTopLeft << trianglePoint2 << trianglePoint3 << triangleTopLeft;
	triangleShadow << triangleTopLeft + shadowOffset << trianglePoint2 + shadowOffset << trianglePoint3 + shadowOffset << triangleTopLeft + shadowOffset;
	
	trianglePath.addPolygon(triangle);
	triangleShadowPath.addPolygon(triangleShadow);
	painter->fillPath(triangleShadowPath, triangleShadowBrush);
	painter->fillPath(trianglePath, triangleBrush);
}

void RosterDelegate::paintShadowText(QPainter* painter, const QRect& region, const QString& text) const {
	painter->setPen(QPen(QColor(254, 254, 254)));
	painter->drawText(region.adjusted(0, 1, 0, 0), Qt::AlignTop, text);
	painter->setPen(QPen(QColor(115, 115, 115)));
	painter->drawText(region, Qt::AlignTop, text);
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
	
	QRect presenceIconRegion(QPoint(farLeftMargin_, fullRegion.top()), QSize(presenceIconWidth_, fullRegion.height() - verticalMargin_));
	
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
	
	QFontMetrics nameMetrics(nameFont_);
	painter->setFont(nameFont_);
	int extraFontWidth = nameMetrics.width("H");
	int leftOffset = avatarRegion.right() + horizontalMargin_ * 2 + extraFontWidth / 2;
	QRect textRegion(fullRegion.adjusted(leftOffset, 0, 0/*-leftOffset*/, 0));
	
	int nameHeight = nameMetrics.height() + verticalMargin_;
	QRect nameRegion(textRegion.adjusted(0, verticalMargin_, 0, 0));
	
	painter->drawText(nameRegion, Qt::AlignTop, index.data(Qt::DisplayRole).toString());
	
	
	painter->setFont(statusFont_);
	painter->setPen(QPen(QColor(160,160,160)));
	
	QRect statusTextRegion(textRegion.adjusted(0, nameHeight, 0, 0));
	painter->drawText(statusTextRegion, Qt::AlignTop, index.data(StatusTextRole).toString());
	
	painter->restore();
}

const int RosterDelegate::avatarSize_ = 20;
const int RosterDelegate::presenceIconHeight_ = 16;
const int RosterDelegate::presenceIconWidth_ = 16;
const int RosterDelegate::groupCornerRadius_ = 0;
const int RosterDelegate::horizontalMargin_ = 2;
const int RosterDelegate::verticalMargin_ = 2;
const int RosterDelegate::farLeftMargin_ = 2;

}
