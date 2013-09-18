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

}

QSize QtCloseButton::sizeHint() const {
	return QSize(style()->pixelMetric(QStyle::PM_TabCloseIndicatorWidth, 0, 0), style()->pixelMetric(QStyle::PM_TabCloseIndicatorHeight, 0, 0));
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
	QStyleOption opt;
	opt.init(this);
	opt.state |= QStyle::State_AutoRaise;
	if (underMouse() && !isDown()) {
		opt.state |= QStyle::State_Raised;
	} else if (isDown()) {
		opt.state |= QStyle::State_Sunken;
	}
	style()->drawPrimitive(QStyle::PE_IndicatorTabClose, &opt, &painter, this);
}

}
