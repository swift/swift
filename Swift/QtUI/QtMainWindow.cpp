#include "QtMainWindow.h"

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

namespace Swift {

QtMainWindow::QtMainWindow(QtTreeWidgetFactory *treeWidgetFactory) : QWidget() {
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->setSpacing(0);
	meView_ = new QtRosterHeader(this);
	mainLayout->addWidget(meView_);
	//statusWidget_ = new QtStatusWidget(this);
	connect(meView_, SIGNAL(onChangeStatusRequest(StatusShow::Type, const QString&)), this, SLOT(handleStatusChanged(StatusShow::Type, const QString&)));
	//mainLayout->addWidget(statusWidget_);
	treeWidget_ = dynamic_cast<QtTreeWidget*>(treeWidgetFactory->createTreeWidget());
	mainLayout->addWidget(treeWidget_);

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

}

