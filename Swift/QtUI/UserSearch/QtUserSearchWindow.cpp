/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/UserSearch/QtUserSearchWindow.h"

#include <qdebug.h>
#include <QModelIndex>

#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UIEvents/RequestChatUIEvent.h"
#include "Swift/Controllers/UIEvents/AddContactUIEvent.h"
#include "Swift/QtUI/UserSearch/UserSearchModel.h"
#include "Swift/QtUI/UserSearch/UserSearchDelegate.h"
#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {

QtUserSearchWindow::QtUserSearchWindow(UIEventStream* eventStream) {
#ifndef Q_WS_MAC
	setWindowIcon(QIcon(":/logo-icon-16.png"));
#endif
	eventStream_ = eventStream;
	setupUi(this);
	model_ = new UserSearchModel();
	delegate_ = new UserSearchDelegate();
	results_->setModel(model_);
	results_->setItemDelegate(delegate_);
	results_->setHeaderHidden(true);
#ifdef SWIFT_PLATFORM_MACOSX
	results_->setAlternatingRowColors(true);
#endif
	connect(service_, SIGNAL(activated(const QString&)), this, SLOT(handleGetForm()));
	connect(getSearchForm_, SIGNAL(clicked()), this, SLOT(handleGetForm()));
	//connect(user_, SIGNAL(returnPressed()), this, SLOT(handleJoin()));
	//connect(nickName_, SIGNAL(returnPressed()), room_, SLOT(setFocus()));
	connect(search_, SIGNAL(clicked()), this, SLOT(handleSearch()));

	connect(results_, SIGNAL(clicked(const QModelIndex&)), this, SLOT(handleSelected(const QModelIndex&)));
	connect(results_, SIGNAL(activated(const QModelIndex&)), this, SLOT(handleActivated(const QModelIndex&)));
	connect(buttonBox_, SIGNAL(accepted()), this, SLOT(handleOkClicked()));
	connect(buttonBox_, SIGNAL(rejected()), this, SLOT(handleCancelClicked()));
	/* When inputs change, check if OK is enabled */
	connect(jid_, SIGNAL(textChanged(const QString&)), this, SLOT(enableCorrectButtons()));
	connect(nickName_, SIGNAL(textChanged(const QString&)), this, SLOT(enableCorrectButtons()));
	connect(startChat_, SIGNAL(stateChanged(int)), this, SLOT(enableCorrectButtons()));
	connect(addToRoster_, SIGNAL(stateChanged(int)), this, SLOT(enableCorrectButtons()));
	enableCorrectButtons();
}

QtUserSearchWindow::~QtUserSearchWindow() {

}

void QtUserSearchWindow::handleGetForm() {
	lastServiceJID_ = JID(Q2PSTRING(service_->currentText()));
	onFormRequested(lastServiceJID_);
}

void QtUserSearchWindow::handleSearch() {
	boost::shared_ptr<SearchPayload> search(new SearchPayload());
	if (nickInput_->isEnabled()) {
		search->setNick(Q2PSTRING(nickInput_->text()));
	}
	if (firstInput_->isEnabled()) {
		search->setFirst(Q2PSTRING(firstInput_->text()));
	}
	if (lastInput_->isEnabled()) {
		search->setLast(Q2PSTRING(lastInput_->text()));
	}
	if (emailInput_->isEnabled()) {
		search->setEMail(Q2PSTRING(emailInput_->text()));
	}
	onSearchRequested(search, lastServiceJID_);
}

void QtUserSearchWindow::show() {
	clear();
	enableCorrectButtons();
	QWidget::show();
}

void QtUserSearchWindow::enableCorrectButtons() {
	bool enable = !jid_->text().isEmpty() && (startChat_->isChecked() || (addToRoster_->isChecked() && !nickName_->text().isEmpty()));
	buttonBox_->button(QDialogButtonBox::Ok)->setEnabled(enable);
}

void QtUserSearchWindow::handleOkClicked() {
	JID contact = JID(Q2PSTRING(jid_->text()));
	String nick = Q2PSTRING(nickName_->text());
	if (addToRoster_->isChecked()) {
		boost::shared_ptr<UIEvent> event(new AddContactUIEvent(contact, nick));
		eventStream_->send(event);
	}
	if (startChat_->isChecked()) {
		boost::shared_ptr<UIEvent> event(new RequestChatUIEvent(contact));
		eventStream_->send(event);
	}
	hide();
}

void QtUserSearchWindow::handleCancelClicked() {
	hide();
}

void QtUserSearchWindow::addSavedServices(const std::vector<JID>& services) {
	service_->clear();
	foreach (JID jid, services) {
		service_->addItem(P2QSTRING(jid.toString()));
	}
	service_->clearEditText();
}

void QtUserSearchWindow::setServerSupportsSearch(bool support) {
	if (!support) {
		stack_->setCurrentIndex(0);
		messageLabel_->setText("This service doesn't support searching for users.");
		search_->setEnabled(false);
	}
}

void QtUserSearchWindow::setSearchFields(boost::shared_ptr<SearchPayload> fields) {
	bool enabled[8] = {fields->getNick(), fields->getNick(), fields->getFirst(), fields->getFirst(), fields->getLast(), fields->getLast(), fields->getEMail(), fields->getEMail()};
	QWidget* widget[8] = {nickInputLabel_, nickInput_, firstInputLabel_, firstInput_, lastInputLabel_, lastInput_, emailInputLabel_, emailInput_};
	for (int i = 0; i < 8; i++) {
		widget[i]->setVisible(enabled[i]);
		widget[i]->setEnabled(enabled[i]);
	}
	stack_->setCurrentIndex(1);
	search_->setEnabled(true);
}

void QtUserSearchWindow::handleActivated(const QModelIndex& index) {
	if (!index.isValid()) {
		return;
	}
	UserSearchResult* userItem = static_cast<UserSearchResult*>(index.internalPointer());
	if (userItem) { /* static cast, so always will be, but if we change to be like mucsearch, remember the check.*/
		handleSelected(index);
		//handleJoin(); /* Don't do anything automatically on selection.*/
	}
}

void QtUserSearchWindow::handleSelected(const QModelIndex& current) {
	if (!current.isValid()) {
		return;
	}
	UserSearchResult* userItem = static_cast<UserSearchResult*>(current.internalPointer());
	if (userItem) { /* Remember to leave this if we change to dynamic cast */
		jid_->setText(P2QSTRING(userItem->getJID().toString()));
	}
}

void QtUserSearchWindow::setResults(const std::vector<UserSearchResult>& results) {
	model_->setResults(results);
}

void QtUserSearchWindow::setSelectedService(const JID& jid) {
	service_->setEditText(P2QSTRING(jid.toString()));
}

void QtUserSearchWindow::clear() {
	stack_->setCurrentIndex(0);
	messageLabel_->setText("Please choose a service to search, above");
	model_->clear();
	search_->setEnabled(false);
}

void QtUserSearchWindow::setSearchError(bool error) {
	//FIXME
}

}
