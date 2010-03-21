#include "QtMainWindow.h"

#include "QtAddContactDialog.h"
#include "QtJoinMUCDialog.h"
#include "QtSwiftUtil.h"
#include "Roster/QtTreeWidgetFactory.h"
#include "Roster/QtTreeWidget.h"

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

namespace Swift {

QtMainWindow::QtMainWindow(UIEventStream* uiEventStream, QtTreeWidgetFactory *treeWidgetFactory) : QWidget() {
	uiEventStream_ = uiEventStream;
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->setSpacing(0);
	meView_ = new QtRosterHeader(this);
	mainLayout->addWidget(meView_);
	connect(meView_, SIGNAL(onChangeStatusRequest(StatusShow::Type, const QString&)), this, SLOT(handleStatusChanged(StatusShow::Type, const QString&)));

	tabs_ = new QTabWidget(this);
	tabs_->setDocumentMode(true);
	tabs_->setTabPosition(QTabWidget::South);
	mainLayout->addWidget(tabs_);
	contactsTabWidget_ = new QWidget(this);
	contactsTabWidget_->setContentsMargins(0, 0, 0, 0);
	QBoxLayout *contactTabLayout = new QBoxLayout(QBoxLayout::TopToBottom, contactsTabWidget_);
	contactsTabWidget_->setLayout(contactTabLayout);
	contactTabLayout->setSpacing(0);
	contactTabLayout->setContentsMargins(0, 0, 0, 0);
	
	treeWidget_ = dynamic_cast<QtTreeWidget*>(treeWidgetFactory->createTreeWidget());
	treeWidget_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	contactTabLayout->addWidget(treeWidget_);

	tabs_->addTab(contactsTabWidget_, "Contacts");
	
	eventWindow_ = new QtEventWindow(uiEventStream_);
	
	tabs_->addTab(eventWindow_, "Events");
	
	this->setLayout(mainLayout);
	
	QMenu* viewMenu = new QMenu(tr("View"), this);
	menus_.push_back(viewMenu);
	QAction* showOfflineAction = new QAction("Show offline contacts", this);
	showOfflineAction->setCheckable(true);
	showOfflineAction->setChecked(false);
	connect(showOfflineAction, SIGNAL(toggled(bool)), SLOT(handleShowOfflineToggled(bool)));
	viewMenu->addAction(showOfflineAction);

	QMenu* chatMenu = new QMenu(tr("Chat"), this);
	menus_.push_back(chatMenu);
	QAction* joinMUCAction = new QAction("Join chatroom", this);
	connect(joinMUCAction, SIGNAL(triggered()), SLOT(handleJoinMUCAction()));
	chatMenu->addAction(joinMUCAction);
	addAction_ = new QAction("Add Contact", this);
	connect(addAction_, SIGNAL(triggered(bool)), this, SLOT(handleAddActionTriggered(bool)));
	chatMenu->addAction(addAction_);
	QAction* signOutAction = new QAction("Sign Out", this);
	connect(signOutAction, SIGNAL(triggered()), SLOT(handleSignOutAction()));
	chatMenu->addAction(signOutAction);
}

QtEventWindow* QtMainWindow::getEventWindow() {
	return eventWindow_;
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
	onAddContactRequest(contact, Q2PSTRING(name));
}

TreeWidget* QtMainWindow::getTreeWidget() {
	return treeWidget_;
}

void QtMainWindow::handleJoinMUCAction() {
	QtJoinMUCDialog* joinMUC = new QtJoinMUCDialog("jabber@conference.jabber.org", "SwiftUser", this);
	connect(joinMUC, SIGNAL(onJoinCommand(const JID&, const QString&)), SLOT(handleJoinMUCDialogComplete(const JID&, const QString&)));
	joinMUC->show();
}

void QtMainWindow::handleJoinMUCDialogComplete(const JID& muc, const QString& nick) {
	onJoinMUCRequest(muc, Q2PSTRING(nick));
}

void QtMainWindow::handleStatusChanged(StatusShow::Type showType, const QString &statusMessage) {
	onChangeStatusRequest(showType, Q2PSTRING(statusMessage));
}

void QtMainWindow::handleShowOfflineToggled(bool state) {
	onShowOfflineToggled(state);
}

void QtMainWindow::setMyName(const String& name) {
	meView_->setName(P2QSTRING(name));
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

}

