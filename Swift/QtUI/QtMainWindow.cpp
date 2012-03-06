/*
 * Copyright (c) 2010-2011 Kevin Smith
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

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtTabWidget.h>
#include <Swift/QtUI/QtSettingsProvider.h>
#include <Swift/QtUI/QtLoginWindow.h>
#include <Roster/QtRosterWidget.h>
#include <Swift/Controllers/UIEvents/RequestJoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestAddUserDialogUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestChatWithUserDialogUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestProfileEditorUIEvent.h>
#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestAdHocUIEvent.h>
#include <Swift/QtUI/QtUISettingConstants.h>
#include <Swift/Controllers/SettingConstants.h>

namespace Swift {

QtMainWindow::QtMainWindow(SettingsProvider* settings, UIEventStream* uiEventStream, QtLoginWindow::QtMenus loginMenus) : QWidget(), MainWindow(false), loginMenus_(loginMenus) {
	uiEventStream_ = uiEventStream;
	settings_ = settings;
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

	treeWidget_ = new QtRosterWidget(uiEventStream_, settings_, this);
	contactTabLayout->addWidget(treeWidget_);

	tabs_->addTab(contactsTabWidget_, tr("&Contacts"));

	eventWindow_ = new QtEventWindow(uiEventStream_);
	connect(eventWindow_, SIGNAL(onNewEventCountUpdated(int)), this, SLOT(handleEventCountUpdated(int)));

	chatListWindow_ = new QtChatListWindow(uiEventStream_, settings_);
	connect(chatListWindow_, SIGNAL(onCountUpdated(int)), this, SLOT(handleChatCountUpdated(int)));

	tabs_->addTab(chatListWindow_, tr("C&hats"));
	tabs_->addTab(eventWindow_, tr("&Notices"));

	tabs_->setCurrentIndex(settings_->getSetting(QtUISettingConstants::CURRENT_ROSTER_TAB));

	connect(tabs_, SIGNAL(currentChanged(int)), this, SLOT(handleTabChanged(int)));

	this->setLayout(mainLayout);

	QMenu* viewMenu = new QMenu(tr("&View"), this);
	menus_.push_back(viewMenu);
	showOfflineAction_ = new QAction(tr("&Show offline contacts"), this);
	showOfflineAction_->setCheckable(true);
	showOfflineAction_->setChecked(false);
	connect(showOfflineAction_, SIGNAL(toggled(bool)), SLOT(handleShowOfflineToggled(bool)));
	viewMenu->addAction(showOfflineAction_);
	handleShowOfflineToggled(settings_->getSetting(SettingConstants::SHOW_OFFLINE));

	//QAction* compactRosterAction_ = new QAction(tr("&Compact Roster"), this);
	//compactRosterAction_->setCheckable(true);
	//compactRosterAction_->setChecked(false);
	//connect(compactRosterAction_, SIGNAL(toggled(bool)), uiPreferences_, SLOT(setCompactRosters(bool)));
	//viewMenu->addAction(compactRosterAction_);

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
	serverAdHocMenu_ = new QMenu(tr("Run Server Command"), this);
	actionsMenu->addMenu(serverAdHocMenu_);
	actionsMenu->addSeparator();
	QAction* signOutAction = new QAction(tr("&Sign Out"), this);
	connect(signOutAction, SIGNAL(triggered()), SLOT(handleSignOutAction()));
	actionsMenu->addAction(signOutAction);

	toggleRequestDeliveryReceipts_ = new QAction(tr("&Request Delivery Receipts"), this);
	toggleRequestDeliveryReceipts_->setCheckable(true);
	toggleRequestDeliveryReceipts_->setChecked(settings_->getSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS));
	connect(toggleRequestDeliveryReceipts_, SIGNAL(toggled(bool)), SLOT(handleToggleRequestDeliveryReceipts(bool)));
	loginMenus_.generalMenu->addAction(toggleRequestDeliveryReceipts_);

	treeWidget_->onSomethingSelectedChanged.connect(boost::bind(&QAction::setEnabled, editUserAction_, _1));

	setAvailableAdHocCommands(std::vector<DiscoItems::Item>());
	QAction* adHocAction = new QAction(tr("Collecting commands..."), this);
	adHocAction->setEnabled(false);
	serverAdHocMenu_->addAction(adHocAction);
	serverAdHocCommandActions_.append(adHocAction);

	settings_->onSettingChanged.connect(boost::bind(&QtMainWindow::handleSettingChanged, this, _1));
}

QtMainWindow::~QtMainWindow() {
	settings_->onSettingChanged.disconnect(boost::bind(&QtMainWindow::handleSettingChanged, this, _1));
}

void QtMainWindow::handleTabChanged(int index) {
	settings_->storeSetting(QtUISettingConstants::CURRENT_ROSTER_TAB, index);
}

void QtMainWindow::handleToggleRequestDeliveryReceipts(bool enabled) {
	settings_->storeSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS, enabled);
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
	int eventIndex = 2;
	tabs_->tabBar()->setTabTextColor(eventIndex, eventTabColor);
	QString text = tr("&Notices");
	if (count > 0) {
		text += QString(" (%1)").arg(count);
	}
	tabs_->setTabText(eventIndex, text);
}

void QtMainWindow::handleChatCountUpdated(int count) {
	QColor chatTabColor = (count == 0) ? QColor() : QColor(255, 0, 0); // invalid resets to default
	int chatIndex = 1;
	tabs_->tabBar()->setTabTextColor(chatIndex, chatTabColor);
	QString text = tr("&Chats");
	if (count > 0) {
		text += QString(" (%1)").arg(count);
	}
	tabs_->setTabText(chatIndex, text);
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
	loginMenus_.generalMenu->removeAction(toggleRequestDeliveryReceipts_);
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

void QtMainWindow::handleSettingChanged(const std::string& settingPath) {
	if (settingPath == SettingConstants::SHOW_OFFLINE.getKey()) {
		handleShowOfflineToggled(settings_->getSetting(SettingConstants::SHOW_OFFLINE));
	}
	if (settingPath == SettingConstants::REQUEST_DELIVERYRECEIPTS.getKey()) {
		toggleRequestDeliveryReceipts_->setChecked(settings_->getSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS));
	}
}

void QtMainWindow::handleShowOfflineToggled(bool state) {
	settings_->storeSetting(SettingConstants::SHOW_OFFLINE, state);
	showOfflineAction_->setChecked(settings_->getSetting(SettingConstants::SHOW_OFFLINE));
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

void QtMainWindow::handleAdHocActionTriggered(bool /*checked*/) {
	QAction* action = qobject_cast<QAction*>(sender());
	assert(action);
	DiscoItems::Item command = serverAdHocCommands_[serverAdHocCommandActions_.indexOf(action)];
	uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestAdHocUIEvent(command)));
}

void QtMainWindow::setAvailableAdHocCommands(const std::vector<DiscoItems::Item>& commands) {
	serverAdHocCommands_ = commands;
	foreach (QAction* action, serverAdHocCommandActions_) {
		delete action;
	}
	serverAdHocMenu_->clear();
	serverAdHocCommandActions_.clear();
	foreach (DiscoItems::Item command, commands) {
		QAction* action = new QAction(P2QSTRING(command.getName()), this);
		connect(action, SIGNAL(triggered(bool)), this, SLOT(handleAdHocActionTriggered(bool)));
		serverAdHocMenu_->addAction(action);
		serverAdHocCommandActions_.append(action);
	}
	if (serverAdHocCommandActions_.isEmpty()) {
		QAction* action = new QAction(tr("No Available Commands"), this);
		action->setEnabled(false);
		serverAdHocMenu_->addAction(action);
		serverAdHocCommandActions_.append(action);
	}
}

}

