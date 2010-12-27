/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "GroupItemDelegate.h"

#include <QPainter>
#include <QPen>
#include <QtDebug>

namespace Swift {

GroupItemDelegate::GroupItemDelegate() : groupFont_(QApplication::font()) {
	groupFont_.setPointSize(common_.nameFont.pointSize() - common_.detailFontSizeDrop);
	groupFont_.setWeight(QFont::Bold);
}

QSize GroupItemDelegate::sizeHint(const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const {
	QFontMetrics groupMetrics(groupFont_);
	return QSize(150, groupMetrics.height() + common_.verticalMargin + 2);
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
	
	int triangleHorizontalOffset = 1;
	int triangleWidth = 9;
	int triangleHeight = 5;
	paintExpansionTriangle(painter, region.adjusted(common_.horizontalMargin +  triangleHorizontalOffset + 1, 0, 0, 0), triangleWidth, triangleHeight, expanded);
	
	int textLeftOffset = 3 * common_.horizontalMargin + 1 + triangleWidth + triangleHorizontalOffset;
	QFontMetrics fontMetrics(groupFont_);
	int textTopOffset = (region.height() - fontMetrics.height()) / 2;
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
		paintShadowText(painter, countRect, countString);
	}
	QRect nameTextRect = expanded ? textRect : textRect.adjusted(0, 0, -contactCountWidth, 0);
	QString elidedName = fontMetrics.elidedText(name, Qt::ElideRight, nameTextRect.width(), Qt::TextShowMnemonic);
	paintShadowText(painter, nameTextRect, elidedName);
	painter->restore();
}

void GroupItemDelegate::paintExpansionTriangle(QPainter* painter, const QRect& region, int width, int height, bool expanded) const {
	// height is the height of the downward pointing triangle
	QPolygonF triangle;
	if (expanded) {
		QPointF triangleTopLeft(region.left(), region.top() + region.height() / 2 - height / 2);
		triangle << triangleTopLeft;
		triangle << triangleTopLeft + QPointF(width, 0);
		triangle << triangleTopLeft + QPointF(width / 2, height);
		// The expanded triangle should be a little lower, because its pointy shape makes it feel
		// as if it's too high.
		triangle.translate(QPointF(0,1));
	}
	else {
		QPointF triangleTopLeft(region.left() + ((width - height) / 2), region.top() + region.height() / 2 - width / 2);
		triangle << triangleTopLeft;
		triangle << triangleTopLeft + QPointF(height, width / 2);
		triangle << triangleTopLeft + QPointF(0, width);
	}
	//qDebug() << "Painting triangle: " << triangle;

	QPolygonF triangleShadow(triangle);
	triangleShadow.translate(QPointF(0, -1));

	QPainterPath trianglePath;
	QPainterPath triangleShadowPath;
	QBrush triangleBrush(QColor(110, 110, 110));
	QBrush triangleShadowBrush(QColor(47, 47, 47));
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
