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

namespace Swift {
QtChatTabs::QtChatTabs() : QWidget() {
	tabs_ = new QtTabWidget(this);
#if QT_VERSION >= 0x040500
	/*For Macs, change the tab rendering.*/
	tabs_->setDocumentMode(true);
	/*Closable tabs are only in Qt4.5 and later*/
	tabs_->setTabsClosable(true);
	connect(tabs_, SIGNAL(tabCloseRequested(int)), this, SLOT(handleTabCloseRequested(int)));
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
	tabs_->addTab(tab, tab->windowTitle());
	connect(tab, SIGNAL(titleUpdated()), this, SLOT(handleTabTitleUpdated()));
	connect(tab, SIGNAL(windowClosing()), this, SLOT(handleTabClosing()));
	connect(tab, SIGNAL(windowOpening()), this, SLOT(handleWidgetShown()));
	connect(tab, SIGNAL(wantsToActivate()), this, SLOT(handleWantsToActivate()));
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
	tabs_->setTabText(index, widget->windowTitle());
	QColor tabTextColor;
	bool flash = false;
	switch (tabbable->getWidgetAlertState()) {
	case QtTabbable::WaitingActivity : flash = true; tabTextColor = QColor(255, 0, 0); break;
	case QtTabbable::ImpendingActivity : tabTextColor = QColor(0, 255, 0); break;
	default : tabTextColor = QColor(-1,-1,-1);//invalid resets to default
	}
	tabs_->tabBar()->setTabTextColor(index, tabTextColor); 
	if (widget == tabs_->currentWidget()) {
		setWindowTitle(widget->windowTitle());
	}

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
