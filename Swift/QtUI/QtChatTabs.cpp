#include "QtChatTabs.h"

#include <algorithm>

#include <QCloseEvent>
#include <QDesktopWidget>
#include <QtGlobal>
#include <QTabWidget>
#include <QLayout>

namespace Swift {
QtChatTabs::QtChatTabs() : QWidget() {
	tabs_ = new QTabWidget(this);
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
		tabs_->removeTab(i);
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
	if (tabs_->indexOf(widget) >= 0) {
		return;
	}
	addTab(widget);
	show();
}

void QtChatTabs::handleWantsToActivate() {
	QtTabbable* widget = qobject_cast<QtTabbable*>(sender());
	Q_ASSERT(widget);
	Q_ASSERT(tabs_->indexOf(widget) >= 0);
	//Un-minimize and bring to front.
	setWindowState(windowState() & ~Qt::WindowMinimized);
	setWindowState(windowState() | Qt::WindowActive);
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
}

void QtChatTabs::handleTabCloseRequested(int index) {
	QWidget* widget = tabs_->widget(index);
	widget->close();
}

void QtChatTabs::handleTabTitleUpdated() {
	QWidget* widget = qobject_cast<QWidget*>(sender());
	if (!widget) {
		return;
	}
	int index = tabs_->indexOf(widget);
	if (index < 0) {
		return;
	}
	tabs_->setTabText(index, widget->windowTitle());
	if (widget == tabs_->currentWidget()) {
		setWindowTitle(widget->windowTitle());
	}
}

void QtChatTabs::resizeEvent(QResizeEvent*) {
	emit geometryChanged();
}

void QtChatTabs::moveEvent(QMoveEvent*) {
	emit geometryChanged();	
}

}
