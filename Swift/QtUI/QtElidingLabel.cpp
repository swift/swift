/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/QtElidingLabel.h"

namespace Swift {
QtElidingLabel::QtElidingLabel(QWidget* parent, Qt::WindowFlags f) : QLabel(parent, f) {
	setSizes();
}

QtElidingLabel::QtElidingLabel(const QString& text, QWidget* parent, Qt::WindowFlags f) : QLabel(text, parent, f) {
	setSizes();
}

QtElidingLabel::~QtElidingLabel() {

}

void QtElidingLabel::setSizes() {
	setMinimumSize(1, minimumHeight());
}

void QtElidingLabel::paintEvent(QPaintEvent* event) {
	//QPainter painter(this);
	QString fullText(text());
	if (fontMetrics().width(fullText) > contentsRect().width()) {
		//QString elidedText(fontMetrics().elidedText(fullText));
		setText(fontMetrics().elidedText(fullText, Qt::ElideRight, rect().width(), Qt::TextShowMnemonic));
	}
	QLabel::paintEvent(event);
	setText(fullText);
}

}
