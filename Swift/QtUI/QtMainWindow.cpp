/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtMainWindow.h"

#include <boost/optional.hpp>
#include <boost/bind.hpp>

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

#include "QtAddContactDialog.h"
#include "QtSwiftUtil.h"
#include "QtTabWidget.h"
#include "Roster/QtTreeWidget.h"
#include "Swift/Controllers/UIEvents/AddContactUIEvent.h"
#include "Swift/Controllers/UIEvents/RequestMUCSearchUIEvent.h"
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
	contextMenu_ = new QtRosterContextMenu(uiEventStream_, treeWidget_);
	treeWidget_->setContextMenu(contextMenu_);
	contactTabLayout->addWidget(treeWidget_);

	tabs_->addTab(contactsTabWidget_, "Contacts");
	
	eventWindow_ = new QtEventWindow(uiEventStream_);
	connect(eventWindow_, SIGNAL(onNewEventCountUpdated(int)), this, SLOT(handleEventCountUpdated(int)));
	
	chatListWindow_ = new QtChatListWindow(uiEventStream_);

	tabs_->addTab(eventWindow_, "Notices");
	tabs_->addTab(chatListWindow_, "Chats");

	this->setLayout(mainLayout);
	
	QMenu* viewMenu = new QMenu(tr("View"), this);
	menus_.push_back(viewMenu);
	showOfflineAction_ = new QAction("Show offline contacts", this);
	showOfflineAction_->setCheckable(true);
	showOfflineAction_->setChecked(false);
	connect(showOfflineAction_, SIGNAL(toggled(bool)), SLOT(handleShowOfflineToggled(bool)));
	viewMenu->addAction(showOfflineAction_);

	QMenu* actionsMenu = new QMenu(tr("Actions"), this);
	menus_.push_back(actionsMenu);
	QAction* joinMUCAction = new QAction("Join Room", this);
	connect(joinMUCAction, SIGNAL(triggered()), SLOT(handleJoinMUCAction()));
	actionsMenu->addAction(joinMUCAction);
	addAction_ = new QAction("Add Contact", this);
	connect(addAction_, SIGNAL(triggered(bool)), this, SLOT(handleAddActionTriggered(bool)));
	actionsMenu->addAction(addAction_);
	QAction* signOutAction = new QAction("Sign Out", this);
	connect(signOutAction, SIGNAL(triggered()), SLOT(handleSignOutAction()));
	actionsMenu->addAction(signOutAction);

	lastOfflineState_ = false;
	uiEventStream_->onUIEvent.connect(boost::bind(&QtMainWindow::handleUIEvent, this, _1));
}

QtMainWindow::~QtMainWindow() {
	uiEventStream_->onUIEvent.disconnect(boost::bind(&QtMainWindow::handleUIEvent, this, _1));
	delete contextMenu_;
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

void QtMainWindow::handleEventCountUpdated(int count) {
	QColor eventTabColor = (count == 0) ? QColor() : QColor(255, 0, 0); // invalid resets to default
	int eventIndex = 1;
	tabs_->tabBar()->setTabTextColor(eventIndex, eventTabColor);
	QString text = "Notices";
	if (count > 0) {
		text += QString(" (%1)").arg(count);
	}
	tabs_->setTabText(eventIndex, text);
}

void QtMainWindow::handleAddActionTriggered(bool checked) {
	Q_UNUSED(checked);
	QtAddContactDialog* addContact = new QtAddContactDialog(this);
	connect(addContact, SIGNAL(onAddCommand(const JID&, const QString&)), SLOT(handleAddContactDialogComplete(const JID&, const QString&)));
	addContact->show();
}

void QtMainWindow::handleSignOutAction() {
	onSignOutRequest();
}

void QtMainWindow::handleAddContactDialogComplete(const JID& contact, const QString& name) {
	boost::shared_ptr<UIEvent> event(new AddContactUIEvent(contact, Q2PSTRING(name)));
	uiEventStream_->send(event);
}

void QtMainWindow::handleJoinMUCAction() {
	uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestMUCSearchUIEvent()));
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

void QtMainWindow::setMyNick(const String& nick) {
	meView_->setNick(P2QSTRING(nick));
}

void QtMainWindow::setMyJID(const JID& jid) {
	meView_->setJID(P2QSTRING(jid.toBare().toString()));
}

void QtMainWindow::setMyAvatarPath(const String& path) {
	meView_->setAvatar(P2QSTRING(path));
}

void QtMainWindow::setMyStatusText(const String& status) {
	meView_->setStatusText(P2QSTRING(status));
}

void QtMainWindow::setMyStatusType(StatusShow::Type type) {
	meView_->setStatusType(type);
}

void QtMainWindow::setConnecting() {
	meView_->setConnecting();
}

}

