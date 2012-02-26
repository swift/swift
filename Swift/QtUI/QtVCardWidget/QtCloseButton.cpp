/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "QtCloseButton.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

namespace Swift {

QtCloseButton::QtCloseButton(QWidget *parent) : QAbstractButton(parent) {
	lightPixmap = QPixmap(12,12);
	lightPixmap.fill(QColor(0,0,0,0));
	QStyleOption opt;
	opt.init(this);
	opt.state = QStyle::State(0);
	opt.state |= QStyle::State_MouseOver;
	QPainter lightPixmapPainter(&lightPixmap);
	style()->drawPrimitive(QStyle::PE_IndicatorTabClose, &opt, &lightPixmapPainter);

	darkPixmap = QPixmap(12,12);
	darkPixmap.fill(QColor(0,0,0,0));
	opt.init(this);
	opt.state = QStyle::State(0);
	QPainter darkPixmapPainter(&darkPixmap);
	style()->drawPrimitive(QStyle::PE_IndicatorTabClose, &opt, &darkPixmapPainter);
}

QSize QtCloseButton::sizeHint() const {
	return QSize(width(), height());
}

bool QtCloseButton::event(QEvent *e) {
	if (e->type() == QEvent::Enter || e->type() == QEvent::Leave) {
		update();
	}
	return QAbstractButton::event(e);
}

void QtCloseButton::paintEvent(QPaintEvent *) {
	QPainter painter(this);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);
	if (underMouse()) {
		painter.drawPixmap(0, 0, height(), height(), darkPixmap);
	} else {
		painter.drawPixmap(0, 0, height(), height(), lightPixmap);
	}
}

}
