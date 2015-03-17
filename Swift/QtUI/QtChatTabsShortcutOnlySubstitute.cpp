/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtChatTabsShortcutOnlySubstitute.h>

#include <cassert>

#include <QApplication>
#include <QShortcut>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/foreach.h>

#include <Swift/QtUI/QtTabbable.h>

namespace Swift {

QtChatTabsShortcutOnlySubstitute::QtChatTabsShortcutOnlySubstitute() : QWidget() {

}

QtChatTabsShortcutOnlySubstitute::~QtChatTabsShortcutOnlySubstitute() {

}

void QtChatTabsShortcutOnlySubstitute::addTab(QtTabbable* tab) {
	connect(tab, SIGNAL(requestNextTab()), this, SLOT(handleRequestedNextTab()), Qt::UniqueConnection);
	connect(tab, SIGNAL(requestActiveTab()), this, SLOT(handleRequestedActiveTab()), Qt::UniqueConnection);
	connect(tab, SIGNAL(requestPreviousTab()), this, SLOT(handleRequestedPreviousTab()), Qt::UniqueConnection);

	connect(new QShortcut(QKeySequence(tr("CTRL+W", "Close chat tab.")), tab), SIGNAL(activated()), this, SLOT(handleCloseTabShortcut()));
	connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageUp), tab), SIGNAL(activated()), tab, SIGNAL(requestPreviousTab()));
	connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageDown), tab), SIGNAL(activated()), tab, SIGNAL(requestNextTab()));
	connect(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_A), tab), SIGNAL(activated()), tab, SIGNAL(requestActiveTab()));
}

void QtChatTabsShortcutOnlySubstitute::handleCloseTabShortcut() {
	QtTabbable* senderTab = dynamic_cast<QtTabbable*>(sender()->parent());
	SWIFT_LOG_ASSERT(senderTab, debug) << "No window to close." << std::endl;
	if (senderTab) {
		senderTab->close();
	}
}

void QtChatTabsShortcutOnlySubstitute::handleRequestedNextTab() {
	QtTabbable* senderTab = dynamic_cast<QtTabbable*>(sender());

	QList<QtTabbable*> tabs = tabbableWindows();

	int currentIndex = tabs.indexOf(senderTab);
	assert(currentIndex >= 0);

	QtTabbable* nextTab = tabs.at((currentIndex + 1) % tabs.size());
	nextTab->activateWindow();
}

void QtChatTabsShortcutOnlySubstitute::handleRequestedActiveTab() {
	QtTabbable* senderTab = dynamic_cast<QtTabbable*>(sender());

	QtTabbable::AlertType types[] = {QtTabbable::WaitingActivity, QtTabbable::ImpendingActivity};

	QList<QtTabbable*> tabs = tabbableWindows();

	for (int j = 0; j < 2; j++) {
		int startIndex = tabs.indexOf(senderTab);
		int currentIndex = startIndex;

		do {
			currentIndex = (currentIndex + 1) % tabs.size();
			QtTabbable* currentTab = tabs.at(currentIndex);
			if (currentTab->getWidgetAlertState() == types[j]) {
				currentTab->activateWindow();
				return;
			}
		} while (startIndex != currentIndex);
	}
}

void QtChatTabsShortcutOnlySubstitute::handleRequestedPreviousTab() {
	QtTabbable* senderTab = dynamic_cast<QtTabbable*>(sender());

	QList<QtTabbable*> tabs = tabbableWindows();

	int currentIndex = tabs.indexOf(senderTab);
	assert(currentIndex >= 0);

	QtTabbable* previousTab = tabs.at((currentIndex + tabs.size() - 1) % tabs.size());
	previousTab->activateWindow();
}

QList<QtTabbable*> QtChatTabsShortcutOnlySubstitute::tabbableWindows() const {
	QList<QWidget*> windows = qApp->topLevelWidgets();

	QList<QtTabbable*> tabbables;
	foreach(QWidget* topLevelWidget, windows) {
		QtTabbable* tabbable = dynamic_cast<QtTabbable*>(topLevelWidget);
		if (tabbable) {
			tabbables << tabbable;
		}
	}

	return tabbables;
}

}

