/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */


#include "Swift/QtUI/QtWebView.h"

#include <QKeyEvent>
#include <QFocusEvent>

namespace Swift {
QtWebView::QtWebView(QWidget* parent) : QWebView(parent) {

}

void QtWebView::keyPressEvent(QKeyEvent* event) {
	Qt::KeyboardModifiers modifiers = event->modifiers();
	int key = event->key();
	if (modifiers == Qt::ShiftModifier && (key == Qt::Key_PageUp || key == Qt::Key_PageDown)) {
		modifiers = Qt::NoModifier;
	}
	QKeyEvent* translatedEvent = new QKeyEvent(QEvent::KeyPress,
							   key,
							   modifiers,
							   event->text(),
							   event->isAutoRepeat(),
							   event->count());
	QWebView::keyPressEvent(translatedEvent);
	delete translatedEvent;
}

void QtWebView::focusInEvent(QFocusEvent* event) {
	QWebView::focusInEvent(event);
	emit gotFocus();
}

}
