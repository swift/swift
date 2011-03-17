/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtMainWindow.h"

#include <boost/optional.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <QBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QTabWidget>

#include "QtSwiftUtil.h"
#include "QtTabWidget.h"
#include "Roster/QtTreeWidget.h"
#include "Swift/Controllers/UIEvents/RequestJoinMUCUIEvent.h"
#include "Swift/Controllers/UIEvents/RequestAddUserDialogUIEvent.h"
#include "Swift/Controllers/UIEvents/RequestChatWithUserDialogUIEvent.h"
#include "Swift/Controllers/UIEvents/RequestProfileEditorUIEvent.h"
#include "Swift/Controllers/UIEvents/JoinMUCUIEvent.h"
#include "Swift/Controllers/UIEvents/ToggleShowOfflineUIEvent.h"

namespace Swift {

QtMainWindow::QtMainWindow(QtSettingsProvider* settings, UIEventStream* uiEventStream) : QWidget(), MainWindow(false) {
	uiEventStream_ = uiEventStream;
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->setSpacing(0);
	meView_ = new QtRosterHeader(settings, this);
	mainLayout->addWidget(meView_);
	connect(meView_, SIGNAL(onChangeStatusRequest(StatusShow::Type, const QString&)), this, SLOT(handleStatusChanged(StatusShow::Type, const QString&)));
	connect(meView_, SIGNAL(onEditProfileRequest()), this, SLOT(handleEditProfileRequest()));

	tabs_ = new QtTabWidget(this);
#if QT_VERSION >= 0x040500
	tabs_->setDocumentMode(true);
#endif
	tabs_->setTabPosition(QTabWidget::South);
	mainLayout->addWidget(tabs_);
	contactsTabWidget_ = new QWidget(this);
	contactsTabWidget_->setContentsMargins(0, 0, 0, 0);
	QBoxLayout *contactTabLayout = new QBoxLayout(QBoxLayout::TopToBottom, contactsTabWidget_);
	contactsTabWidget_->setLayout(contactTabLayout);
	contactTabLayout->setSpacing(0);
	contactTabLayout->setContentsMargins(0, 0, 0, 0);

	treeWidget_ = new QtTreeWidget(uiEventStream_);
	treeWidget_->setEditable(true);
	contactTabLayout->addWidget(treeWidget_);

	tabs_->addTab(contactsTabWidget_, tr("&Contacts"));

	eventWindow_ = new QtEventWindow(uiEventStream_);
	connect(eventWindow_, SIGNAL(onNewEventCountUpdated(int)), this, SLOT(handleEventCountUpdated(int)));

	chatListWindow_ = new QtChatListWindow(uiEventStream_);

	tabs_->addTab(eventWindow_, tr("&Notices"));
	tabs_->addTab(chatListWindow_, tr("C&hats"));

	this->setLayout(mainLayout);

	QMenu* viewMenu = new QMenu(tr("&View"), this);
	menus_.push_back(viewMenu);
	showOfflineAction_ = new QAction(tr("&Show offline contacts"), this);
	showOfflineAction_->setCheckable(true);
	showOfflineAction_->setChecked(false);
	connect(showOfflineAction_, SIGNAL(toggled(bool)), SLOT(handleShowOfflineToggled(bool)));
	viewMenu->addAction(showOfflineAction_);

	QMenu* actionsMenu = new QMenu(tr("&Actions"), this);
	menus_.push_back(actionsMenu);
	QAction* editProfileAction = new QAction(tr("Edit &Profile"), this);
	connect(editProfileAction, SIGNAL(triggered()), SLOT(handleEditProfileAction()));
	actionsMenu->addAction(editProfileAction);
	QAction* joinMUCAction = new QAction(tr("Enter &Room"), this);
	connect(joinMUCAction, SIGNAL(triggered()), SLOT(handleJoinMUCAction()));
	actionsMenu->addAction(joinMUCAction);
	addUserAction_ = new QAction(tr("&Add Contact"), this);
	connect(addUserAction_, SIGNAL(triggered(bool)), this, SLOT(handleAddUserActionTriggered(bool)));
	actionsMenu->addAction(addUserAction_);
	editUserAction_ = new QAction(tr("&Edit Selected Contact"), this);
	connect(editUserAction_, SIGNAL(triggered(bool)), treeWidget_, SLOT(handleEditUserActionTriggered(bool)));
	actionsMenu->addAction(editUserAction_);
	editUserAction_->setEnabled(false);
	chatUserAction_ = new QAction(tr("Start &Chat"), this);
	connect(chatUserAction_, SIGNAL(triggered(bool)), this, SLOT(handleChatUserActionTriggered(bool)));
	actionsMenu->addAction(chatUserAction_);
	actionsMenu->addSeparator();
	QAction* signOutAction = new QAction(tr("&Sign Out"), this);
	connect(signOutAction, SIGNAL(triggered()), SLOT(handleSignOutAction()));
	actionsMenu->addAction(signOutAction);

	connect(treeWidget_, SIGNAL(onSomethingSelectedChanged(bool)), editUserAction_, SLOT(setEnabled(bool)));

	lastOfflineState_ = false;
	uiEventStream_->onUIEvent.connect(boost::bind(&QtMainWindow::handleUIEvent, this, _1));
}

QtMainWindow::~QtMainWindow() {
	uiEventStream_->onUIEvent.disconnect(boost::bind(&QtMainWindow::handleUIEvent, this, _1));
}

QtEventWindow* QtMainWindow::getEventWindow() {
	return eventWindow_;
}

QtChatListWindow* QtMainWindow::getChatListWindow() {
	return chatListWindow_;
}

void QtMainWindow::setRosterModel(Roster* roster) {
	treeWidget_->setRosterModel(roster);
}

void QtMainWindow::handleEditProfileRequest() {
	uiEventStream_->send(boost::make_shared<RequestProfileEditorUIEvent>());
}

void QtMainWindow::handleEventCountUpdated(int count) {
	QColor eventTabColor = (count == 0) ? QColor() : QColor(255, 0, 0); // invalid resets to default
	int eventIndex = 1;
	tabs_->tabBar()->setTabTextColor(eventIndex, eventTabColor);
	QString text = tr("&Notices");
	if (count > 0) {
		text += QString(" (%1)").arg(count);
	}
	tabs_->setTabText(eventIndex, text);
}

void QtMainWindow::handleAddUserActionTriggered(bool /*checked*/) {
	boost::shared_ptr<UIEvent> event(new RequestAddUserDialogUIEvent());
	uiEventStream_->send(event);
}

void QtMainWindow::handleChatUserActionTriggered(bool /*checked*/) {
	boost::shared_ptr<UIEvent> event(new RequestChatWithUserDialogUIEvent());
	uiEventStream_->send(event);
}

void QtMainWindow::handleSignOutAction() {
	onSignOutRequest();
}

void QtMainWindow::handleEditProfileAction() {
	uiEventStream_->send(boost::make_shared<RequestProfileEditorUIEvent>());
}

void QtMainWindow::handleJoinMUCAction() {
	uiEventStream_->send(boost::make_shared<RequestJoinMUCUIEvent>());
}

void QtMainWindow::handleStatusChanged(StatusShow::Type showType, const QString &statusMessage) {
	onChangeStatusRequest(showType, Q2PSTRING(statusMessage));
}

void QtMainWindow::handleUIEvent(boost::shared_ptr<UIEvent> event) {
	boost::shared_ptr<ToggleShowOfflineUIEvent> toggleEvent = boost::dynamic_pointer_cast<ToggleShowOfflineUIEvent>(event);
	if (toggleEvent) {
		handleShowOfflineToggled(toggleEvent->getShow());
	}
}

void QtMainWindow::handleShowOfflineToggled(bool state) {
	if (state != lastOfflineState_) {
		lastOfflineState_ = state;
		showOfflineAction_->setChecked(state);
		uiEventStream_->onUIEvent(boost::shared_ptr<UIEvent>(new ToggleShowOfflineUIEvent(state)));
	}
}

void QtMainWindow::setMyNick(const std::string& nick) {
	meView_->setNick(P2QSTRING(nick));
}

void QtMainWindow::setMyJID(const JID& jid) {
	meView_->setJID(P2QSTRING(jid.toBare().toString()));
}

void QtMainWindow::setMyAvatarPath(const std::string& path) {
	meView_->setAvatar(P2QSTRING(path));
}

void QtMainWindow::setMyStatusText(const std::string& status) {
	meView_->setStatusText(P2QSTRING(status));
}

void QtMainWindow::setMyStatusType(StatusShow::Type type) {
	meView_->setStatusType(type);
}

void QtMainWindow::setConnecting() {
	meView_->setConnecting();
}


}

