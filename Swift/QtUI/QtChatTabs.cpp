/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtChatTabs.h"

#include <algorithm>
#include <vector>

#include <Swift/Controllers/ChatMessageSummarizer.h>
#include <Swift/QtUI/QtSwiftUtil.h>

#include <QCloseEvent>
#include <QDesktopWidget>
#include <QtGlobal>
#include <QTabWidget>
#include <QLayout>
#include <QTabBar>
#include <QApplication>
#include <qdebug.h>

namespace Swift {
QtChatTabs::QtChatTabs() : QWidget() {
#ifndef Q_WS_MAC
	setWindowIcon(QIcon(":/logo-chat-16.png"));
#endif

	tabs_ = new QtTabWidget(this);
	tabs_->setUsesScrollButtons(true);
	tabs_->setElideMode(Qt::ElideRight);
#if QT_VERSION >= 0x040500
	/*For Macs, change the tab rendering.*/
	tabs_->setDocumentMode(true);
	/*Closable tabs are only in Qt4.5 and later*/
	tabs_->setTabsClosable(true);
	connect(tabs_, SIGNAL(tabCloseRequested(int)), this, SLOT(handleTabCloseRequested(int)));
#else
#warning Qt 4.5 or later is needed. Trying anyway, some things will be disabled.
#endif
	QVBoxLayout *layout = new QVBoxLayout;
	layout->setSpacing(0);
	layout->setContentsMargins(0, 3, 0, 0);
	layout->addWidget(tabs_);
	setLayout(layout);
	//resize(400, 300);
}

void QtChatTabs::closeEvent(QCloseEvent* event) {
	//Hide first to prevent flickering as each tab is removed.
	hide();
	for (int i = tabs_->count() - 1; i >= 0; i--) {
		tabs_->widget(i)->close();
	}
	event->accept();
}

QtTabbable* QtChatTabs::getCurrentTab() {
	return qobject_cast<QtTabbable*>(tabs_->currentWidget());
}

void QtChatTabs::addTab(QtTabbable* tab) {
	QSizePolicy policy = sizePolicy();
	/* Chat windows like to grow - don't let them */
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	tabs_->addTab(tab, tab->windowTitle());
	connect(tab, SIGNAL(titleUpdated()), this, SLOT(handleTabTitleUpdated()), Qt::UniqueConnection);
	connect(tab, SIGNAL(countUpdated()), this, SLOT(handleTabTitleUpdated()), Qt::UniqueConnection);
	connect(tab, SIGNAL(windowClosing()), this, SLOT(handleTabClosing()), Qt::UniqueConnection);
	connect(tab, SIGNAL(windowOpening()), this, SLOT(handleWidgetShown()), Qt::UniqueConnection);
	connect(tab, SIGNAL(wantsToActivate()), this, SLOT(handleWantsToActivate()), Qt::UniqueConnection);
	connect(tab, SIGNAL(requestNextTab()), this, SLOT(handleRequestedNextTab()), Qt::UniqueConnection);
	connect(tab, SIGNAL(requestActiveTab()), this, SLOT(handleRequestedActiveTab()), Qt::UniqueConnection);
	connect(tab, SIGNAL(requestPreviousTab()), this, SLOT(handleRequestedPreviousTab()), Qt::UniqueConnection);
	connect(tab, SIGNAL(requestFlash()), this, SLOT(flash()), Qt::UniqueConnection);
	setSizePolicy(policy);
}

void QtChatTabs::handleWidgetShown() {
	QtTabbable* widget = qobject_cast<QtTabbable*>(sender());
	if (!widget) {
		return;
	}
	checkForFirstShow();
	if (tabs_->indexOf(widget) >= 0) {
		handleTabTitleUpdated(widget);
		return;
	}
	addTab(widget);
	show();
}

void QtChatTabs::handleWantsToActivate() {
	QtTabbable* widget = qobject_cast<QtTabbable*>(sender());
	Q_ASSERT(widget);
	//Un-minimize and bring to front.
	setWindowState(windowState() & ~Qt::WindowMinimized);
	setWindowState(windowState() | Qt::WindowActive);
	show();
	widget->show();
	tabs_->setCurrentWidget(widget);
	handleTabTitleUpdated(widget);
	widget->setFocus();
	raise();
	activateWindow();
}

void QtChatTabs::handleTabClosing() {
	QWidget* widget = qobject_cast<QWidget*>(sender());
	if (!widget) {
		return;
	}
	int index = tabs_->indexOf(widget);
	if (index < 0) {
		return;
	}
	tabs_->removeTab(index);
	if (tabs_->count() == 0) {
		hide();
	}
	handleTabTitleUpdated(tabs_->currentWidget());
}

void QtChatTabs::handleRequestedPreviousTab() {
	int newIndex = tabs_->currentIndex() - 1;
	tabs_->setCurrentIndex(newIndex >= 0 ? newIndex : tabs_->count() - 1);
}

void QtChatTabs::handleRequestedNextTab() {
	int newIndex = tabs_->currentIndex() + 1;
	tabs_->setCurrentIndex(newIndex < tabs_->count() ? newIndex : 0);
}

void QtChatTabs::handleRequestedActiveTab() {
	QtTabbable::AlertType types[] = {QtTabbable::WaitingActivity, QtTabbable::ImpendingActivity};
	bool finished = false;
	for (int j = 0; j < 2; j++) {
		bool looped = false;
		for (int i = tabs_->currentIndex() + 1; !finished && i != tabs_->currentIndex(); i++) {
			if (i >= tabs_->count()) {
				if (looped) {
					break;
				}
				looped = true;
				i = 0;
			}
			if (qobject_cast<QtTabbable*>(tabs_->widget(i))->getWidgetAlertState() == types[j]) {
				tabs_->setCurrentIndex(i);
				finished = true;
				break;
			}
		}
	}
}


void QtChatTabs::handleTabCloseRequested(int index) {
	QWidget* widget = tabs_->widget(index);
	widget->close();
}

void QtChatTabs::handleTabTitleUpdated() {
	QWidget* widget = qobject_cast<QWidget*>(sender());
	handleTabTitleUpdated(widget);
}

void QtChatTabs::handleTabTitleUpdated(QWidget* widget) {
	if (!widget) {
		return;
	}
	QtTabbable* tabbable = qobject_cast<QtTabbable*>(widget);
	int index = tabs_->indexOf(widget);
	if (index < 0) {
		return;
	}

	QString tabText = tabbable->windowTitle().simplified();

	// look for spectrum-generated and other long JID localparts, and
	// try to abbreviate the resulting long tab texts
	QRegExp hasTrailingGarbage("^(.[-\\w\\s&]+)([^\\s\\w].*)$");
	if (hasTrailingGarbage.exactMatch(tabText) &&
	    hasTrailingGarbage.cap(1).simplified().length() >= 2 &&
	    hasTrailingGarbage.cap(2).length() >= 7) {
		// there may be some trailing garbage, and it's long
		// enough to be worth removing, and we'd leave at
		// least a couple of characters.
		tabText = hasTrailingGarbage.cap(1).simplified();
	}

	// QTabBar interprets &, so escape that
	tabText.replace("&", "&&");

	// see which alt[a-z] keys other tabs use
	bool accelsTaken[26];
	int i = 0;
	while (i < 26) {
		accelsTaken[i] = (i == 0); //A is used for 'switch to active tab'
		i++;
	}
	int other = tabs_->tabBar()->count();
	while (other >= 0) {
		other--;
		if (other != index) {
			QString t = tabs_->tabBar()->tabText(other).toLower();
			int r = t.indexOf('&');
			if (r >= 0 && t[r+1] >= 'a' && t[r+1] <= 'z') {
				accelsTaken[t[r+1].unicode()-'a'] = true;
			}
		}
	}
	// then look to see which letters in tabText may be used
	i = 0;
	int accelPos = -1;
	while (i < tabText.length() && accelPos < 0) {
		if (tabText[i] >= 'A' && tabText[i] <= 'Z' &&
		     !accelsTaken[tabText[i].unicode()-'A']) {
			accelPos = i;
		}
		if (tabText[i] >= 'a' && tabText[i] <= 'z' &&
		     !accelsTaken[tabText[i].unicode()-'a']) {
			accelPos = i;
		}
		++i;
	}
	if (accelPos >= 0) {
		tabText = tabText.mid(0, accelPos) + "&" + tabText.mid(accelPos);
	}
	// this could be improved on some european keyboards, such as
	// the German one (where alt-Sz-Ligature is available) and basically
	// doesn't work on Arabic/Indic keyboards (where Latin letters
	// aren't available), but I don't care to deal with those.

	tabs_->setTabText(index, tabbable->getCount() > 0 ? QString("(%1) %2").arg(tabbable->getCount()).arg(tabText) : tabText);
	QColor tabTextColor;
	switch (tabbable->getWidgetAlertState()) {
	case QtTabbable::WaitingActivity : tabTextColor = QColor(217, 20, 43); break;
	case QtTabbable::ImpendingActivity : tabTextColor = QColor(27, 171, 32); break;
	default : tabTextColor = QColor();
	}
	tabs_->tabBar()->setTabTextColor(index, tabTextColor);

	std::vector<std::pair<std::string, int> > unreads;
	for (int i = 0; i < tabs_->count(); i++) {
		QtTabbable* tab = qobject_cast<QtTabbable*>(tabs_->widget(i));
		if (tab) {
			unreads.push_back(std::pair<std::string, int>(Q2PSTRING(tab->windowTitle()), tab->getCount()));
		}
	}

	std::string current(Q2PSTRING(qobject_cast<QtTabbable*>(tabs_->currentWidget())->windowTitle()));
	ChatMessageSummarizer summary;
	setWindowTitle(summary.getSummary(current, unreads).c_str());
}

void QtChatTabs::flash() {
#ifndef SWIFTEN_PLATFORM_MACOSX
	QApplication::alert(this, 0);
#endif
}

void QtChatTabs::resizeEvent(QResizeEvent*) {
	emit geometryChanged();
}

void QtChatTabs::moveEvent(QMoveEvent*) {
	emit geometryChanged();
}

void QtChatTabs::checkForFirstShow() {
	if (!isVisible()) {
		showMinimized();
	}
}

}
