/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/QtElidingLabel.h"

namespace Swift {
QtElidingLabel::QtElidingLabel(QWidget* parent, Qt::WindowFlags f) : QLabel(parent, f) {
	fullText_ = "";
	dirty_ = true;
	setSizes();
	setTextFormat(Qt::PlainText);
}

QtElidingLabel::QtElidingLabel(const QString& text, QWidget* parent, Qt::WindowFlags f) : QLabel(text, parent, f) {
	fullText_ = text;
	dirty_ = true;
	setSizes();
	setTextFormat(Qt::PlainText);
}

QtElidingLabel::~QtElidingLabel() {

}

void QtElidingLabel::setSizes() {
	setMinimumSize(1, minimumHeight());
}

void QtElidingLabel::setText(const QString& text) {
	fullText_ = text;
	QLabel::setText(text);
	dirty_ = true;
}

void QtElidingLabel::paintEvent(QPaintEvent* event) {
	QRect rect = contentsRect();
	dirty_ = dirty_ || rect != lastRect_;
	if (dirty_) {
		lastRect_ = rect;
		int fontWidth = fontMetrics().width(fullText_);
		if (fontWidth > rect.width()) {
			QString elidedText(fontMetrics().elidedText(fullText_, Qt::ElideRight, rect.width(), Qt::TextShowMnemonic));
			QLabel::setText(elidedText);
		} else {
			QLabel::setText(fullText_);
		}
		dirty_ = false;
	}
	QLabel::paintEvent(event);
}

}
