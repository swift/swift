/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtTabbable.h"

#include <QApplication>

#include "QtChatTabs.h"

namespace Swift {

QtTabbable::~QtTabbable() {
	emit windowClosing();
}

bool QtTabbable::isWidgetSelected() {
	/*isActiveWindow() shouldn't be necessary, but I don't trust it as far as I can throw it*/
	if (!isActiveWindow()) {
		return false;
	}
	QtChatTabs* parent = qobject_cast<QtChatTabs*>(window());
	return parent ? parent->getCurrentTab() == this : isAncestorOf(QApplication::focusWidget());
}

void QtTabbable::keyPressEvent(QKeyEvent *event) {
	handleKeyPressEvent(event);
}

void QtTabbable::handleKeyPressEvent(QKeyEvent *event) {
	event->ignore();
	int key = event->key();
	Qt::KeyboardModifiers modifiers = event->modifiers();
	if (key == Qt::Key_W && modifiers == Qt::ControlModifier) {
		close();
		event->accept();
	} else if (
		(key == Qt::Key_PageUp && modifiers == Qt::ControlModifier)
//		|| (key == Qt::Key_Left && modifiers == (Qt::ControlModifier & Qt::ShiftModifier))
	) {
		emit requestPreviousTab();
		event->accept();
	} else if (
		(key == Qt::Key_PageDown && modifiers == Qt::ControlModifier)
//		|| (key == Qt::Key_Right && modifiers == (Qt::ControlModifier & Qt::ShiftModifier)
		|| (key == Qt::Key_Tab && modifiers == Qt::ControlModifier)
	) {
		emit requestNextTab();
		event->accept();
	} else if (
		(key == Qt::Key_A && modifiers == Qt::AltModifier)
	)  {
		emit requestActiveTab();
		event->accept();
	}
}

}
