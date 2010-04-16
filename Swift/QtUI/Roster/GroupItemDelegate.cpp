/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "GroupItemDelegate.h"

#include <QPainter>
#include <QPen>

namespace Swift {

GroupItemDelegate::GroupItemDelegate() : groupFont_(QApplication::font()) {
	groupFont_.setPointSize(common_.nameFont.pointSize() - common_.detailFontSizeDrop);
	groupFont_.setWeight(QFont::Bold);
}

QSize GroupItemDelegate::sizeHint(const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const {
	QFontMetrics groupMetrics(groupFont_);
	return QSize(150, groupMetrics.height() + 4);
}

void GroupItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QString& name, int rowCount, bool expanded) const {
	painter->save();		
	painter->setPen(QPen(QColor(189, 189, 189)));
	//FIXME: It looks like Qt is passing us a rectangle that's too small
	//This deliberately draws outside the lines, and we need to find a better solution.
	int correctionAmount = groupCornerRadius_ > 0 ? 0 : 1;
	QRect region(QPoint(option.rect.left() - correctionAmount, option.rect.top()), QSize(option.rect.width() + correctionAmount, option.rect.height() - common_.verticalMargin));
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
	paintExpansionTriangle(painter, region.adjusted(common_.horizontalMargin +  1, 0, 0, 0), triangleWidth, triangleHeight, expanded);
	
	int textLeftOffset = 3 * common_.horizontalMargin + 1 + triangleWidth;
	QFontMetrics fontMetrics(groupFont_);
	int textTopOffset = (option.rect.height() - fontMetrics.height()) / 2;
	painter->setFont(groupFont_);
	int contactCountWidth = 0;
	QRect textRect = region.adjusted(textLeftOffset, textTopOffset, -1 * textLeftOffset, -1 * textTopOffset);

	if (!expanded) {
		QFontMetrics groupMetrics(groupFont_);
		int contactCount = rowCount;
		QString countString = QString("%1").arg(contactCount);
		contactCountWidth = groupMetrics.width(countString) + 2 * common_.horizontalMargin;
		int offsetAmount = textRect.width() - contactCountWidth + common_.horizontalMargin;
		QRect countRect = textRect.adjusted(offsetAmount, 0, 0/*-1 * offsetAmount*/, 0);
		//qDebug() << "Painting count string " << countString << " at " << countRect << " from offset " << offsetAmount;
		paintShadowText(painter, countRect, countString);
	}
	QRect nameTextRect = textRect.adjusted(0, 0, contactCountWidth, 0);
	paintShadowText(painter, nameTextRect, name);
	painter->restore();
}

void GroupItemDelegate::paintExpansionTriangle(QPainter* painter, const QRect& region, int width, int height, bool expanded) const {
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

void GroupItemDelegate::paintShadowText(QPainter* painter, const QRect& region, const QString& text) const {
	painter->setPen(QPen(QColor(254, 254, 254)));
	painter->drawText(region.adjusted(0, 1, 0, 0), Qt::AlignTop, text);
	painter->setPen(QPen(QColor(115, 115, 115)));
	painter->drawText(region, Qt::AlignTop, text);
}

const int GroupItemDelegate::groupCornerRadius_ = 0;

}
