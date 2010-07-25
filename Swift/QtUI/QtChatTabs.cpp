/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtChatTabs.h"

#include <algorithm>

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

void QtChatTabs::addTab(QtTabbable* tab) {
	QSizePolicy policy = sizePolicy();
	/* Chat windows like to grow - don't let them */
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	tabs_->addTab(tab, tab->windowTitle());
	connect(tab, SIGNAL(titleUpdated()), this, SLOT(handleTabTitleUpdated()));
	connect(tab, SIGNAL(countUpdated()), this, SLOT(handleTabTitleUpdated()));
	connect(tab, SIGNAL(windowClosing()), this, SLOT(handleTabClosing()));
	connect(tab, SIGNAL(windowOpening()), this, SLOT(handleWidgetShown()));
	connect(tab, SIGNAL(wantsToActivate()), this, SLOT(handleWantsToActivate()));
	connect(tab, SIGNAL(requestNextTab()), this, SLOT(handleRequestedNextTab()));
	connect(tab, SIGNAL(requestActiveTab()), this, SLOT(handleRequestedActiveTab()));
	connect(tab, SIGNAL(requestPreviousTab()), this, SLOT(handleRequestedPreviousTab()));
	setSizePolicy(policy);
}

void QtChatTabs::handleWidgetShown() {
	QtTabbable* widget = qobject_cast<QtTabbable*>(sender());
	if (!widget) {
		return;
	}
	checkForFirstShow();
	if (tabs_->indexOf(widget) >= 0) {
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
	widget->setFocus();
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
	tabs_->setTabText(index, tabbable->getCount() > 0 ? QString("(%1) %2").arg(tabbable->getCount()).arg(tabbable->windowTitle()) : tabbable->windowTitle());
	QColor tabTextColor;
	bool flash = false;
	switch (tabbable->getWidgetAlertState()) {
	case QtTabbable::WaitingActivity : flash = true; tabTextColor = QColor(217, 20, 43); break;
	case QtTabbable::ImpendingActivity : tabTextColor = QColor(27, 171, 32); break;
	default : tabTextColor = QColor();
	}
	tabs_->tabBar()->setTabTextColor(index, tabTextColor); 
	int unread = 0;
	for (int i = 0; i < tabs_->count(); i++) {
		QtTabbable* tab = qobject_cast<QtTabbable*>(tabs_->widget(i));
		if (tab) {
			unread += tab->getCount(); 
		}
	}
	
	QtTabbable* current = qobject_cast<QtTabbable*>(tabs_->currentWidget());
	setWindowTitle(unread > 0 ? QString("(%1) %2").arg(unread).arg(current->windowTitle()) : current->windowTitle());

	if (flash) {
#ifndef SWIFTEN_PLATFORM_MACOSX
		QApplication::alert(this, 3000);
#endif
	}
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
