/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/UserSearch/QtUserSearchWindow.h"

#include <qdebug.h>
#include <QModelIndex>
#include <QWizardPage>
#include <QMovie>

#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UIEvents/RequestChatUIEvent.h"
#include "Swift/Controllers/UIEvents/AddContactUIEvent.h"
#include "Swift/QtUI/UserSearch/UserSearchModel.h"
#include "Swift/QtUI/UserSearch/UserSearchDelegate.h"
#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {

QtUserSearchWindow::QtUserSearchWindow(UIEventStream* eventStream, UserSearchWindow::Type type) : type_(type) {
#ifndef Q_WS_MAC
	setWindowIcon(QIcon(":/logo-icon-16.png"));
#endif
	eventStream_ = eventStream;
	setupUi(this);
	model_ = new UserSearchModel();
	delegate_ = new UserSearchDelegate();
	firstPage_ = new QtUserSearchFirstPage();
	connect(firstPage_->byJID_, SIGNAL(toggled(bool)), this, SLOT(handleFirstPageRadioChange()));
	connect(firstPage_->byLocalSearch_, SIGNAL(toggled(bool)), this, SLOT(handleFirstPageRadioChange()));
	connect(firstPage_->byRemoteSearch_, SIGNAL(toggled(bool)), this, SLOT(handleFirstPageRadioChange()));
	fieldsPage_ = new QtUserSearchFieldsPage();
	fieldsPage_->fetchingThrobber_->setMovie(new QMovie(":/icons/throbber.gif", QByteArray(), this));
	fieldsPage_->fetchingThrobber_->movie()->stop();
	resultsPage_ = new QtUserSearchResultsPage();
	resultsPage_->results_->setModel(model_);
	resultsPage_->results_->setItemDelegate(delegate_);
	resultsPage_->results_->setHeaderHidden(true);
	setPage(1, firstPage_);
	setPage(2, fieldsPage_);
	setPage(3, resultsPage_);

#ifdef SWIFT_PLATFORM_MACOSX
	resultsPage_->results_->setAlternatingRowColors(true);
#endif
	connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(handleCurrentChanged(int)));
	connect(this, SIGNAL(accepted()), this, SLOT(handleAccepted()));
	clear();
}

QtUserSearchWindow::~QtUserSearchWindow() {

}

void QtUserSearchWindow::handleCurrentChanged(int page) {
	qDebug() << "Next called, currently, was " << currentId() << ", " << lastPage_;
	if (page == 2 && lastPage_ == 1) {
		setError("");
		/* next won't be called if JID is selected */
		JID server = searchServer();
		clearForm();
		onFormRequested(server);
	} else if (page == 3 && lastPage_ == 2) {
		handleSearch();
	}
	lastPage_ = page;
}

JID QtUserSearchWindow::searchServer() {
	return firstPage_->byRemoteSearch_->isChecked() ? JID(Q2PSTRING(firstPage_->service_->currentText())) : myServer_;
}

void QtUserSearchWindow::handleAccepted() {
	JID jid;
	if (!firstPage_->byJID_->isChecked()) {
		UserSearchResult* userItem = static_cast<UserSearchResult*>(resultsPage_->results_->currentIndex().internalPointer());
		if (userItem) { /* Remember to leave this if we change to dynamic cast */
			jid = userItem->getJID();
		}
	} else {
		jid = JID(Q2PSTRING(firstPage_->jid_->text()));
	}
	if (type_ == AddContact) {
		/* FIXME: allow specifying a nick */
		boost::shared_ptr<UIEvent> event(new AddContactUIEvent(jid, jid.toString()));
		eventStream_->send(event);
	} else {
		boost::shared_ptr<UIEvent> event(new RequestChatUIEvent(jid));
		eventStream_->send(event);
	}
}

int QtUserSearchWindow::nextId() const {
	switch (currentId()) {
		case 1: return firstPage_->byJID_->isChecked() ? -1 : 2;
		case 2: return 3;
		case 3: return -1;
		default: return -1;
	}
}

void QtUserSearchWindow::handleFirstPageRadioChange() {
	if (firstPage_->byJID_->isChecked()) {
		firstPage_->jidLabel_->setText("What is their JID? (This has the form 'alice@wonderland.lit')");
		firstPage_->jidLabel_->show();
		firstPage_->service_->hide();
		firstPage_->jid_->setText("");
		firstPage_->jid_->show();
		restart();
	} else if (firstPage_->byRemoteSearch_->isChecked()) {
		firstPage_->jidLabel_->setText("Which server do you want to search? (This has the form 'wonderland.lit')");
		firstPage_->jidLabel_->hide();
		firstPage_->service_->show();
		firstPage_->service_->setEditText("");
		//firstPage_->jid_->setText("");
		firstPage_->jid_->hide();
		restart();
	} else {
		firstPage_->jid_->hide();
		firstPage_->jidLabel_->hide();
		firstPage_->service_->hide();
		restart();
	}
}

void QtUserSearchWindow::handleSearch() {
	boost::shared_ptr<SearchPayload> search(new SearchPayload());
	if (fieldsPage_->nickInput_->isEnabled()) {
		search->setNick(Q2PSTRING(fieldsPage_->nickInput_->text()));
	}
	if (fieldsPage_->firstInput_->isEnabled()) {
		search->setFirst(Q2PSTRING(fieldsPage_->firstInput_->text()));
	}
	if (fieldsPage_->lastInput_->isEnabled()) {
		search->setLast(Q2PSTRING(fieldsPage_->lastInput_->text()));
	}
	if (fieldsPage_->emailInput_->isEnabled()) {
		search->setEMail(Q2PSTRING(fieldsPage_->emailInput_->text()));
	}
	onSearchRequested(search, searchServer());
}

void QtUserSearchWindow::show() {
	clear();
	QWidget::show();
}
//
//void QtUserSearchWindow::enableCorrectButtons() {
//	bool enable = !jid_->text().isEmpty() && (startChat_->isChecked() || (addToRoster_->isChecked() && !nickName_->text().isEmpty()));
//	buttonBox_->button(QDialogButtonBox::Ok)->setEnabled(enable);
//}
//
//void QtUserSearchWindow::handleOkClicked() {
//	JID contact = JID(Q2PSTRING(jid_->text()));
//	String nick = Q2PSTRING(nickName_->text());
//	if (addToRoster_->isChecked()) {
//		boost::shared_ptr<UIEvent> event(new AddContactUIEvent(contact, nick));
//		eventStream_->send(event);
//	}
//	if (startChat_->isChecked()) {
//		boost::shared_ptr<UIEvent> event(new RequestChatUIEvent(contact));
//		eventStream_->send(event);
//	}
//	hide();
//}
//
//void QtUserSearchWindow::handleCancelClicked() {
//	hide();
//}
//
void QtUserSearchWindow::addSavedServices(const std::vector<JID>& services) {
	firstPage_->service_->clear();
	foreach (JID jid, services) {
		firstPage_->service_->addItem(P2QSTRING(jid.toString()));
	}
	firstPage_->service_->clearEditText();
}

void QtUserSearchWindow::setSearchFields(boost::shared_ptr<SearchPayload> fields) {
	fieldsPage_->fetchingThrobber_->hide();
	fieldsPage_->fetchingThrobber_->movie()->stop();
	fieldsPage_->fetchingLabel_->hide();
	bool enabled[8] = {fields->getNick(), fields->getNick(), fields->getFirst(), fields->getFirst(), fields->getLast(), fields->getLast(), fields->getEMail(), fields->getEMail()};
	QWidget* legacySearchWidgets[8] = {fieldsPage_->nickInputLabel_, fieldsPage_->nickInput_, fieldsPage_->firstInputLabel_, fieldsPage_->firstInput_, fieldsPage_->lastInputLabel_, fieldsPage_->lastInput_, fieldsPage_->emailInputLabel_, fieldsPage_->emailInput_};
	for (int i = 0; i < 8; i++) {
		legacySearchWidgets[i]->setVisible(enabled[i]);
		legacySearchWidgets[i]->setEnabled(enabled[i]);
	}
}
//
//void QtUserSearchWindow::handleActivated(const QModelIndex& index) {
//	if (!index.isValid()) {
//		return;
//	}
//	UserSearchResult* userItem = static_cast<UserSearchResult*>(index.internalPointer());
//	if (userItem) { /* static cast, so always will be, but if we change to be like mucsearch, remember the check.*/
//		handleSelected(index);
//		//handleJoin(); /* Don't do anything automatically on selection.*/
//	}
//}
//
//void QtUserSearchWindow::handleSelected(const QModelIndex& current) {
//	if (!current.isValid()) {
//		return;
//	}
//	UserSearchResult* userItem = static_cast<UserSearchResult*>(current.internalPointer());
//	if (userItem) { /* Remember to leave this if we change to dynamic cast */
//		jid_->setText(P2QSTRING(userItem->getJID().toString()));
//	}
//}
//
void QtUserSearchWindow::setResults(const std::vector<UserSearchResult>& results) {
	model_->setResults(results);
}

void QtUserSearchWindow::setSelectedService(const JID& jid) {
	myServer_ = jid;
}

void QtUserSearchWindow::clearForm() {
	fieldsPage_->fetchingThrobber_->show();
	fieldsPage_->fetchingThrobber_->movie()->start();
	fieldsPage_->fetchingLabel_->show();
	QWidget* legacySearchWidgets[8] = {fieldsPage_->nickInputLabel_, fieldsPage_->nickInput_, fieldsPage_->firstInputLabel_, fieldsPage_->firstInput_, fieldsPage_->lastInputLabel_, fieldsPage_->lastInput_, fieldsPage_->emailInputLabel_, fieldsPage_->emailInput_};
	for (int i = 0; i < 8; i++) {
		legacySearchWidgets[i]->hide();
	}
}

void QtUserSearchWindow::clear() {
	firstPage_->errorLabel_->setVisible(false);
	firstPage_->howLabel_->setText(QString("How would you like to find the user to %1?").arg(type_ == AddContact ? "add" : "chat to"));
	firstPage_->byJID_->setChecked(true);
	clearForm();
	model_->clear();
	handleFirstPageRadioChange();
	restart();
	lastPage_ = 1;
}

void QtUserSearchWindow::setError(const QString& error) {
	if (error.isEmpty()) {
		firstPage_->errorLabel_->hide();
	} else {
		firstPage_->errorLabel_->setText(QString("<font color='red'>%1</font>").arg(error));
		firstPage_->errorLabel_->show();
		restart();
		lastPage_ = 1;
	}
}

void QtUserSearchWindow::setSearchError(bool error) {
	if (error) {
		setError("Error while searching");
	}
}

void QtUserSearchWindow::setServerSupportsSearch(bool support) {
	if (!support) {
		setError("This server doesn't support searching for users.");
	}
}

}
