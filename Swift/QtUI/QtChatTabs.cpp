/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtChatTabs.h>

#include <algorithm>
#include <vector>

#include <Swiften/Base/Log.h>

#include <Swift/Controllers/ChatMessageSummarizer.h>
#include <Swift/Controllers/SettingConstants.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtTabWidget.h>
#include <Swift/QtUI/QtTabbable.h>
#include <Swift/QtUI/Trellis/QtDynamicGridLayout.h>
#include <Swift/QtUI/Trellis/QtGridSelectionDialog.h>

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QCursor>
#include <QDesktopWidget>
#include <QLayout>
#include <QMenu>
#include <QTabBar>
#include <QTabWidget>
#include <QtGlobal>

namespace Swift {
QtChatTabs::QtChatTabs(bool singleWindow, SettingsProvider* settingsProvider, bool trellisMode) : QWidget(), singleWindow_(singleWindow), settingsProvider_(settingsProvider), trellisMode_(trellisMode), dynamicGrid_(NULL), gridSelectionDialog_(NULL) {
#ifndef Q_OS_MAC
	setWindowIcon(QIcon(":/logo-chat-16.png"));
#else
	setAttribute(Qt::WA_ShowWithoutActivating);
#endif
	dynamicGrid_ = new QtDynamicGridLayout(this, trellisMode);
	connect(dynamicGrid_, SIGNAL(tabCloseRequested(int)), this, SLOT(handleTabCloseRequested(int)));
	connect(dynamicGrid_, SIGNAL(onCurrentIndexChanged(int)), this, SLOT(handleCurrentTabIndexChanged(int)));

	QVBoxLayout *layout = new QVBoxLayout;
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(dynamicGrid_);
	setLayout(layout);

	if (trellisMode) {
		// restore size
		std::string gridSizeString = settingsProvider->getSetting(SettingConstants::TRELLIS_GRID_SIZE);
		if (!gridSizeString.empty()) {
			QByteArray gridSizeData = QByteArray::fromBase64(P2QSTRING(gridSizeString).toUtf8());
			QDataStream dataStreamGridSize(&gridSizeData, QIODevice::ReadWrite);
			QSize gridSize(1,1);
			dataStreamGridSize >> gridSize;
			dynamicGrid_->setDimensions(gridSize);
		}

		// restore positions
		std::string tabPositionsString = settingsProvider->getSetting(SettingConstants::TRELLIS_GRID_POSITIONS);
		if (!tabPositionsString.empty()) {
			QByteArray tabPositionsData = QByteArray::fromBase64(P2QSTRING(tabPositionsString).toUtf8());
			QDataStream inTabPositions(&tabPositionsData, QIODevice::ReadWrite);
			QHash<QString, QPoint> tabPositions;
			inTabPositions >> tabPositions;
			dynamicGrid_->setTabPositions(tabPositions);
		}
	}

	gridSelectionDialog_ = new QtGridSelectionDialog();

	// setup shortcuts
	shortcuts_ << new QShortcut(QKeySequence(tr("CTRL+W", "Close chat tab.")), window(), SLOT(handleCloseTabShortcut()));
	shortcuts_ << new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageUp), window(), SLOT(handleRequestedPreviousTab()));
	shortcuts_ << new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageDown), window(), SLOT(handleRequestedNextTab()));
	shortcuts_ << new QShortcut(QKeySequence(Qt::ALT + Qt::Key_A), window(), SLOT(handleRequestedActiveTab()));
}

QtChatTabs::~QtChatTabs() {
	foreach (QShortcut* shortcut, shortcuts_) {
		delete shortcut;
	}

	if (trellisMode_) {
		storeTabPositions();
	}
	delete gridSelectionDialog_;
}

void QtChatTabs::closeEvent(QCloseEvent* event) {
	//Hide first to prevent flickering as each tab is removed.
	hide();
	if (trellisMode_) {
		storeTabPositions();
	}

	for (int i = dynamicGrid_->count() - 1; i >= 0; i--) {
		dynamicGrid_->widget(i)->close();
	}
	event->accept();
}

QtTabbable* QtChatTabs::getCurrentTab() {
	return qobject_cast<QtTabbable*>(dynamicGrid_->currentWidget());
}

void QtChatTabs::setViewMenu(QMenu* viewMenu) {
	if (trellisMode_) {
		viewMenu->addSeparator();
		QAction* action = new QAction(tr("Change &layout"), this);
		action->setShortcutContext(Qt::ApplicationShortcut);
		action->setShortcut(QKeySequence(tr("Ctrl+Alt+L")));
		connect(action, SIGNAL(triggered()), this, SLOT(handleOpenLayoutChangeDialog()));
		viewMenu->addAction(action);

		action = new QAction(tr("Move Tab right"), this);
		action->setShortcutContext(Qt::ApplicationShortcut);
		action->setShortcut(QKeySequence(tr("Ctrl+Shift+PgDown")));
		connect(action, SIGNAL(triggered()), dynamicGrid_, SLOT(moveCurrentTabRight()));
		viewMenu->addAction(action);

		action = new QAction(tr("Move Tab left"), this);
		action->setShortcutContext(Qt::ApplicationShortcut);
		action->setShortcut(QKeySequence(tr("Ctrl+Shift+PgUp")));
		connect(action, SIGNAL(triggered()), dynamicGrid_, SLOT(moveCurrentTabLeft()));
		viewMenu->addAction(action);

		action = new QAction(tr("Move Tab to next group"), this);
		action->setShortcutContext(Qt::ApplicationShortcut);
		action->setShortcut(QKeySequence(tr("Ctrl+Alt+PgDown")));
		connect(action, SIGNAL(triggered()), dynamicGrid_, SLOT(moveCurrentTabToNextGroup()));
		viewMenu->addAction(action);

		action = new QAction(tr("Move Tab to previous group"), this);
		action->setShortcutContext(Qt::ApplicationShortcut);
		action->setShortcut(QKeySequence(tr("Ctrl+Alt+PgUp")));
		connect(action, SIGNAL(triggered()), dynamicGrid_, SLOT(moveCurrentTabToPreviousGroup()));
		viewMenu->addAction(action);
	}
}

void QtChatTabs::addTab(QtTabbable* tab) {
	QSizePolicy policy = sizePolicy();
	/* Chat windows like to grow - don't let them */
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	dynamicGrid_->addTab(tab, tab->windowTitle());
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
	if (dynamicGrid_->indexOf(widget) >= 0) {
		handleTabTitleUpdated(widget);
		return;
	}
	widget->blockSignals(true);
	addTab(widget);
	widget->blockSignals(false);
	show();
}

void QtChatTabs::handleCurrentTabIndexChanged(int newIndex) {
	handleTabTitleUpdated(dynamicGrid_->widget(newIndex));
}

void QtChatTabs::handleWantsToActivate() {
	QtTabbable* widget = qobject_cast<QtTabbable*>(sender());
	Q_ASSERT(widget);
	//Un-minimize and bring to front.
	setWindowState(windowState() & ~Qt::WindowMinimized);
	setWindowState(windowState() | Qt::WindowActive);
	show();
	widget->show();
	dynamicGrid_->setCurrentWidget(widget);
	handleTabTitleUpdated(widget);
	widget->setFocus();
	raise();
	activateWindow();
}

void QtChatTabs::handleTabClosing() {
	QWidget* widget = qobject_cast<QWidget*>(sender());
	int index;
	if (widget && ((index = dynamicGrid_->indexOf(widget)) >= 0)) {
		dynamicGrid_->removeTab(index);
		if (dynamicGrid_->count() == 0) {
			if (!singleWindow_) {
				hide();
			}
			else {
				setWindowTitle("");
				onTitleChanged("");
			}
		}
		else {
			handleTabTitleUpdated(dynamicGrid_->currentWidget());
		}
	}
}

void QtChatTabs::handleRequestedPreviousTab() {
	int newIndex = dynamicGrid_->currentIndex() - 1;
	dynamicGrid_->setCurrentIndex(newIndex >= 0 ? newIndex : dynamicGrid_->count() - 1);
}

void QtChatTabs::handleRequestedNextTab() {
	int newIndex = dynamicGrid_->currentIndex() + 1;
	dynamicGrid_->setCurrentIndex(newIndex < dynamicGrid_->count() ? newIndex : 0);
}

void QtChatTabs::handleRequestedActiveTab() {
	QtTabbable::AlertType types[] = {QtTabbable::WaitingActivity, QtTabbable::ImpendingActivity};
	bool finished = false;
	for (int j = 0; j < 2; j++) {
		bool looped = false;
		for (int i = dynamicGrid_->currentIndex() + 1; !finished && i != dynamicGrid_->currentIndex(); i++) {
			if (i >= dynamicGrid_->count()) {
				if (looped) {
					break;
				}
				looped = true;
				i = 0;
			}
			if (qobject_cast<QtTabbable*>(dynamicGrid_->widget(i))->getWidgetAlertState() == types[j]) {
				dynamicGrid_->setCurrentIndex(i);
				finished = true;
				break;
			}
		}
	}
}


void QtChatTabs::handleCloseTabShortcut() {
	QWidget* currentWidget = dynamicGrid_->currentWidget();
	if (currentWidget) {
		currentWidget->close();
	}
}

void QtChatTabs::handleTabCloseRequested(int index) {
	if (trellisMode_) {
		storeTabPositions();
	}

	assert(index < dynamicGrid_->count());
	QWidget* widget = dynamicGrid_->widget(index);
	assert(widget);
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
	int index = dynamicGrid_->indexOf(widget);
	if (index < 0) {
		return;
	}

	QString tabText = tabbable->windowTitle().simplified();
	// look for spectrum-generated and other long JID localparts, and
	// try to abbreviate the resulting long tab texts
	QRegExp hasTrailingGarbage("^(.[-\\w\\s,&]+)([^\\s\\,w].*)$");
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

	int other = dynamicGrid_->count();
	while (other >= 0) {
		other--;
		if (other != index) {
			int tabIndex = -1;
			QtTabWidget* tabWidget = dynamicGrid_->indexToTabWidget(other, tabIndex);
			QString t = tabWidget->tabBar()->tabText(tabIndex).toLower();
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

	int tabIndex = -1;
	QtTabWidget* tabWidget = dynamicGrid_->indexToTabWidget(index, tabIndex);
	tabWidget->setTabText(tabIndex, tabbable->getCount() > 0 ? QString("(%1) %2").arg(tabbable->getCount()).arg(tabText) : tabText);
	QColor tabTextColor;
	switch (tabbable->getWidgetAlertState()) {
	case QtTabbable::WaitingActivity : tabTextColor = QColor(217, 20, 43); break;
	case QtTabbable::ImpendingActivity : tabTextColor = QColor(27, 171, 32); break;
	case QtTabbable::NoActivity : tabTextColor = QColor(); break;
	}
	tabWidget->tabBar()->setTabTextColor(tabIndex, tabTextColor);

	std::vector<std::pair<std::string, int> > unreads;
	for (int i = 0; i < dynamicGrid_->count(); i++) {
		QtTabbable* tab = qobject_cast<QtTabbable*>(dynamicGrid_->widget(i));
		if (tab) {
			unreads.push_back(std::pair<std::string, int>(Q2PSTRING(tab->windowTitle()), tab->getCount()));
		}
	}

	std::string current(Q2PSTRING(qobject_cast<QtTabbable*>(dynamicGrid_->currentWidget())->windowTitle()));
	ChatMessageSummarizer summary;
	QString title = summary.getSummary(current, unreads).c_str();
	setWindowTitle(title);
	emit onTitleChanged(title);
}

void QtChatTabs::flash() {
#ifndef SWIFTEN_PLATFORM_MACOSX
	QApplication::alert(this, 0);
#endif
}

void QtChatTabs::handleOpenLayoutChangeDialog() {
	disconnect(gridSelectionDialog_, SIGNAL(currentGridSizeChanged(QSize)), dynamicGrid_, SLOT(setDimensions(QSize)));
	gridSelectionDialog_->setCurrentGridSize(dynamicGrid_->getDimension());
	gridSelectionDialog_->move(QCursor::pos());
	connect(gridSelectionDialog_, SIGNAL(currentGridSizeChanged(QSize)), dynamicGrid_, SLOT(setDimensions(QSize)));
	gridSelectionDialog_->show();
}

void QtChatTabs::storeTabPositions() {
	// save size
	QByteArray gridSizeData;
	QDataStream dataStreamGridSize(&gridSizeData, QIODevice::ReadWrite);
	dataStreamGridSize << dynamicGrid_->getDimension();
	settingsProvider_->storeSetting(SettingConstants::TRELLIS_GRID_SIZE, Q2PSTRING(QString(gridSizeData.toBase64())));

	// save positions
	QByteArray tabPositionsData;
	QDataStream dataStreamTabPositions(&tabPositionsData, QIODevice::ReadWrite);
	dynamicGrid_->updateTabPositions();
	dataStreamTabPositions << dynamicGrid_->getTabPositions();
	settingsProvider_->storeSetting(SettingConstants::TRELLIS_GRID_POSITIONS, Q2PSTRING(QString(tabPositionsData.toBase64())));
}

void QtChatTabs::resizeEvent(QResizeEvent*) {
	emit geometryChanged();
}

void QtChatTabs::moveEvent(QMoveEvent*) {
	emit geometryChanged();
}

void QtChatTabs::checkForFirstShow() {
	if (!isVisible()) {
#ifndef Q_OS_MAC
		showMinimized();
#else
		/* https://bugreports.qt-project.org/browse/QTBUG-19194 
		 * ^ When the above is fixed we can swap the below for just show();
		 *  WA_ShowWithoutActivating seems to helpfully not work, so... */

		QWidget* currentWindow = QApplication::activeWindow(); /* Remember who had focus if we're the current application*/
		show();
		QCoreApplication::processEvents(); /* Run through the eventloop to clear the show() */
		if (currentWindow) {
			currentWindow->activateWindow(); /* Set focus back */
		}
#endif
	}
}

}
