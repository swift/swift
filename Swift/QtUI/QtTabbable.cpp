/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtTabbable.h>

#include <QApplication>
#include <QKeyEvent>
#include <QShortcut>

#include <Swiften/Base/foreach.h>
#include <Swiften/Base/Platform.h>

#include <Swift/QtUI/QtChatTabs.h>
#include <Swift/QtUI/QtUtilities.h>

namespace Swift {

QtTabbable::QtTabbable() : QWidget() {

}

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

bool QtTabbable::event(QEvent* event) {
	QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
	if (keyEvent) {
		// According to Qt's focus documentation, one can only override CTRL+TAB via reimplementing QWidget::event().
		if (keyEvent->modifiers().testFlag(QtUtilities::ctrlHardwareKeyModifier) && keyEvent->key() == Qt::Key_Tab) {
			if (keyEvent->type() != QEvent::ShortcutOverride) {
				emit requestNextTab();
			}
			return true;
		}
#ifdef SWIFTEN_PLATFORM_LINUX
		else if (keyEvent->modifiers().testFlag(QtUtilities::ctrlHardwareKeyModifier) && keyEvent->key() == Qt::Key_Backtab && keyEvent->type() != QEvent::KeyRelease) {
#else
		else if (keyEvent->modifiers().testFlag(QtUtilities::ctrlHardwareKeyModifier) && keyEvent->key() == Qt::Key_Backtab) {
#endif
			if (keyEvent->type() != QEvent::ShortcutOverride) {
				emit requestPreviousTab();
			}
			return true;
		}
	}
	return QWidget::event(event);
}

}
