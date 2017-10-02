/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/UserSearch/QtUserSearchWindow.h>

#include <memory>

#include <boost/bind.hpp>

#include <QItemDelegate>
#include <QModelIndex>
#include <QMovie>
#include <QWizardPage>

#include <Swift/Controllers/UIEvents/AddContactUIEvent.h>
#include <Swift/Controllers/UIEvents/CreateImpromptuMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/InviteToMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestChatUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>

#include <Swift/QtUI/QtFormResultItemModel.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/UserSearch/QtContactListWidget.h>
#include <Swift/QtUI/UserSearch/QtUserSearchDetailsPage.h>
#include <Swift/QtUI/UserSearch/QtUserSearchFieldsPage.h>
#include <Swift/QtUI/UserSearch/QtUserSearchFirstMultiJIDPage.h>
#include <Swift/QtUI/UserSearch/QtUserSearchFirstPage.h>
#include <Swift/QtUI/UserSearch/QtUserSearchResultsPage.h>
#include <Swift/QtUI/UserSearch/UserSearchDelegate.h>
#include <Swift/QtUI/UserSearch/UserSearchModel.h>

namespace Swift {

QtUserSearchWindow::QtUserSearchWindow(UIEventStream* eventStream, UserSearchWindow::Type type, const std::set<std::string>& groups, SettingsProvider* settingsProvider) : eventStream_(eventStream), type_(type), model_(nullptr), firstPage_(nullptr), firstMultiJIDPage_(nullptr), settings_(settingsProvider), searchNext_(false), supportsImpromptu_(false) {
    setupUi(this);
#ifndef Q_OS_MAC
#ifdef  Q_OS_WIN32
    setWindowIcon(QIcon(":/logo-icon-16-win.png"));
#else
    setWindowIcon(QIcon(":/logo-icon-16.png"));
#endif
#endif
    QString title;
    switch(type) {
        case Type::AddContact:
            title = tr("Add Contact");
            break;
        case Type::ChatToContact:
            title = tr("Chat to Users");
            break;
        case Type::InviteToChat:
            title = tr("Add Users to Chat");
            break;
    }
    setWindowTitle(title);

    delegate_ = new UserSearchDelegate(this);

    setFirstPage(title);
    setSecondPage();
    setThirdPage();

    detailsPage_ = new QtUserSearchDetailsPage(groups);
    setPage(4, detailsPage_);

    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(handleCurrentChanged(int)));
    connect(this, SIGNAL(accepted()), this, SLOT(handleAccepted()));
    clear();
}

QtUserSearchWindow::~QtUserSearchWindow() {
    delete model_;
}

void QtUserSearchWindow::handleCurrentChanged(int page) {
    searchNext_ = false;

    // Check preconditions per type.
    if (type_ == Type::AddContact) {
        assert(firstPage_);
        assert(!firstMultiJIDPage_);
    }
    else {
        assert(!firstPage_);
        assert(firstMultiJIDPage_);
    }

    if (type_ != Type::AddContact) {
        firstMultiJIDPage_->reset();
    }
    resultsPage_->emitCompletenessCheck();
    if (type_ != Type::AddContact && page == 1 && lastPage_ == 3) {
        addSearchedJIDToList(getContact());
        setSecondPage();
    }
    else if (page == 2 && lastPage_ == 1) {
        setError("");
        /* next won't be called if JID is selected */
        JID server = getServerToSearch();
        clearForm();
        onFormRequested(server);
        setThirdPage();
    }
    else if (page == 3 && lastPage_ == 2) {
        JID server = getServerToSearch();
        handleSearch();

        if (type_ == Type::AddContact) {
            bool remote = firstPage_->byRemoteSearch_->isChecked();
            firstPage_->byRemoteSearch_->setChecked(remote);
            firstPage_->service_->setEditText(P2QSTRING(server.toString()));
        } else {
            bool remote = firstMultiJIDPage_->byRemoteSearch_->isChecked();
            setFirstPage();
            firstMultiJIDPage_->byRemoteSearch_->setChecked(remote);
            if (remote) {
                firstMultiJIDPage_->service_->setEditText(P2QSTRING(server.toString()));
            }
        }
    }
    else if (page == 4) {
        detailsPage_->clear();
        detailsPage_->setJID(getContactJID());
        onNameSuggestionRequested(getContactJID());
    }
    lastPage_ = page;
}

JID QtUserSearchWindow::getServerToSearch() {
    if (type_ == Type::AddContact) {
        return firstPage_->byRemoteSearch_->isChecked() ? JID(Q2PSTRING(firstPage_->service_->currentText().trimmed())) : myServer_;
    }
    else {
        return firstMultiJIDPage_->byRemoteSearch_->isChecked() ? JID(Q2PSTRING(firstMultiJIDPage_->service_->currentText().trimmed())) : myServer_;
    }
}

void QtUserSearchWindow::handleAccepted() {
    JID jid;
    std::vector<JID> jids;
    switch(type_) {
        case Type::AddContact:
            jid = getContactJID();
            eventStream_->send(std::make_shared<AddContactUIEvent>(jid, detailsPage_->getName(), detailsPage_->getSelectedGroups()));
            break;
        case Type::ChatToContact:
            if (contactVector_.size() == 1) {
                std::shared_ptr<UIEvent> event(new RequestChatUIEvent(contactVector_[0]->jid));
                eventStream_->send(event);
                break;
            }

            for (Contact::ref contact : contactVector_) {
                jids.push_back(contact->jid);
            }

            eventStream_->send(std::make_shared<CreateImpromptuMUCUIEvent>(jids, JID(), Q2PSTRING(firstMultiJIDPage_->reason_->text())));
            break;
        case Type::InviteToChat:
            for (Contact::ref contact : contactVector_) {
                jids.push_back(contact->jid);
            }
            eventStream_->send(std::make_shared<InviteToMUCUIEvent>(originatorJID_, jids, Q2PSTRING(firstMultiJIDPage_->reason_->text())));
            break;
    }
}

void QtUserSearchWindow::handleContactSuggestionRequested(const QString& text) {
    std::string stdText = Q2PSTRING(text);
    onContactSuggestionsRequested(stdText);
}

void QtUserSearchWindow::addContact() {
    auto contactToAdd = firstMultiJIDPage_->jid_->getContact();
    if (!!contactToAdd) {
        contactVector_.push_back(contactToAdd);
        firstMultiJIDPage_->jid_->clear();
    }

    firstMultiJIDPage_->contactList_->setList(contactVector_);
    firstMultiJIDPage_->emitCompletenessCheck();
    if (type_ == Type::ChatToContact) {
        firstMultiJIDPage_->groupBox->setEnabled(supportsImpromptu_ ? 1 : (contactVector_.size() < 1));
    }

    if (!!contactToAdd && contactToAdd->jid.isValid() && contactToAdd->statusType == StatusShow::None) {
        onJIDUpdateRequested({contactToAdd->jid});
    }
}

void QtUserSearchWindow::setWarning(const boost::optional<std::string>& message) {
    if (message) {
        firstPage_->jidWarning_->setToolTip(P2QSTRING((*message)));
        firstPage_->jidWarning_->setAccessibleDescription(P2QSTRING((*message)));
        firstPage_->jidWarning_->show();
    }
    else {
        firstPage_->jidWarning_->setToolTip("");
        firstPage_->jidWarning_->setAccessibleDescription("");
        firstPage_->jidWarning_->hide();
    }
    firstPage_->emitCompletenessCheck();
}

int QtUserSearchWindow::nextId() const {
    if (type_ == Type::AddContact) {
        switch (currentId()) {
            case 1: return firstPage_->byJID_->isChecked() ? (type_ == Type::AddContact ? 4 : -1) : 2;
            case 2: return 3;
            case 3: return type_ == Type::AddContact ? 4 : -1;
            case 4: return -1;
            default: return -1;
        }
    } else {
        switch (currentId()) {
            case 1: return searchNext_ ? 2 : -1;
            case 2: return 3;
            case 3: return 1;
            case 4: return -1;
            default: return -1;
        }
    }
}

void QtUserSearchWindow::handleFirstPageRadioChange() {
    if (firstPage_->byJID_->isChecked()) {
        firstPage_->jid_->setText("");
        firstPage_->jid_->setEnabled(true);
        firstPage_->service_->setEnabled(false);
        restart();
    }
    else if (firstPage_->byRemoteSearch_->isChecked()) {
        firstPage_->service_->setEditText("");
        firstPage_->jid_->setEnabled(false);
        firstPage_->service_->setEnabled(true);
        //firstPage_->jid_->setText("");
        restart();
    }
    else {
        firstPage_->jid_->setEnabled(false);
        firstPage_->service_->setEnabled(false);
        restart();
    }
}

void QtUserSearchWindow::handleSearch() {
    std::shared_ptr<SearchPayload> search(new SearchPayload());
    if (fieldsPage_->getFormWidget()) {
        search->setForm(fieldsPage_->getFormWidget()->getCompletedForm());
        search->getForm()->clearEmptyTextFields();
    } else {
        if (fieldsPage_->nickInput_->isEnabled() && !fieldsPage_->nickInput_->text().isEmpty()) {
            search->setNick(Q2PSTRING(fieldsPage_->nickInput_->text()));
        }
        if (fieldsPage_->firstInput_->isEnabled() && !fieldsPage_->firstInput_->text().isEmpty()) {
            search->setFirst(Q2PSTRING(fieldsPage_->firstInput_->text()));
        }
        if (fieldsPage_->lastInput_->isEnabled() && !fieldsPage_->lastInput_->text().isEmpty()) {
            search->setLast(Q2PSTRING(fieldsPage_->lastInput_->text()));
        }
        if (fieldsPage_->emailInput_->isEnabled() && !fieldsPage_->emailInput_->text().isEmpty()) {
            search->setEMail(Q2PSTRING(fieldsPage_->emailInput_->text()));
        }
    }
    onSearchRequested(search, getServerToSearch());
}

JID QtUserSearchWindow::getContactJID() const {
    JID jid;

    bool useSearchResult;
    if (type_ == Type::AddContact) {
        useSearchResult = !firstPage_->byJID_->isChecked();
    } else {
        useSearchResult = true;
    }

    if (useSearchResult) {
        if (dynamic_cast<UserSearchModel*>(model_)) {
            UserSearchResult* userItem = static_cast<UserSearchResult*>(resultsPage_->results_->currentIndex().internalPointer());
            if (userItem) { /* Remember to leave this if we change to dynamic cast */
                jid = userItem->getJID();
            }
        } else if (dynamic_cast<QtFormResultItemModel*>(model_)) {
            int row = resultsPage_->results_->currentIndex().row();

            Form::FormItem item = dynamic_cast<QtFormResultItemModel*>(model_)->getForm()->getItems().at(row);
            JID fallbackJid;
            for (FormField::ref field : item) {
                if (field->getType() == FormField::JIDSingleType) {
                    jid = JID(field->getJIDSingleValue());
                    break;
                }
                if (field->getName() == "jid") {
                    fallbackJid = field->getValues()[0];
                }
            }
            if (!jid.isValid()) {
                jid = fallbackJid;
            }
        }
    }
    else {
        jid = JID(Q2PSTRING(firstPage_->jid_->text().trimmed()));
    }
    return jid;
}

Contact::ref QtUserSearchWindow::getContact() const {
    return std::make_shared<Contact>("", getContactJID(), StatusShow::None, "");
}

void QtUserSearchWindow::addSearchedJIDToList(const Contact::ref& contact) {
    std::vector<JID> jids;
    jids.push_back(contact->jid);
    handleJIDsAdded(jids);
    firstMultiJIDPage_->jid_->clear();
}

void QtUserSearchWindow::handleOnSearchedJIDSelected(const Contact::ref& contact) {
    firstPage_->jid_->setText(P2QSTRING(contact->jid.toString()));
}

void QtUserSearchWindow::show() {
    clear();
    if (type_ == Type::AddContact) {
        setWarning(boost::optional<std::string>());
    }
    QWidget::show();
    raise();
}

void QtUserSearchWindow::addSavedServices(const std::vector<JID>& services) {
    if (type_ == Type::AddContact) {
        firstPage_->service_->clear();
        for (auto&& jid : services) {
            firstPage_->service_->addItem(P2QSTRING(jid.toString()));
        }
        firstPage_->service_->clearEditText();
    } else {
        firstMultiJIDPage_->service_->clear();
        for (auto&& jid : services) {
            firstMultiJIDPage_->service_->addItem(P2QSTRING(jid.toString()));
        }
        firstMultiJIDPage_->service_->clearEditText();
    }
}

void QtUserSearchWindow::setSearchFields(std::shared_ptr<SearchPayload> fields) {
    fieldsPage_->fetchingThrobber_->hide();
    fieldsPage_->fetchingThrobber_->movie()->stop();
    fieldsPage_->fetchingLabel_->hide();

    fieldsPage_->instructionsLabel_->setText(fields->getInstructions() ? P2QSTRING(fields->getInstructions().get()) : "Enter search terms");
    if (fields->getForm()) {
        fieldsPage_->setFormWidget(new QtFormWidget(fields->getForm(), fieldsPage_));
    } else {
        fieldsPage_->setFormWidget(nullptr);
        bool enabled[8] = {!!fields->getNick(), !!fields->getNick(), !!fields->getFirst(), !!fields->getFirst(), !!fields->getLast(), !!fields->getLast(), !!fields->getEMail(), !!fields->getEMail()};
        QWidget* legacySearchWidgets[8] = {fieldsPage_->nickInputLabel_, fieldsPage_->nickInput_, fieldsPage_->firstInputLabel_, fieldsPage_->firstInput_, fieldsPage_->lastInputLabel_, fieldsPage_->lastInput_, fieldsPage_->emailInputLabel_, fieldsPage_->emailInput_};
        for (int i = 0; i < 8; i++) {
            legacySearchWidgets[i]->setVisible(enabled[i]);
            legacySearchWidgets[i]->setEnabled(enabled[i]);
        }
    }
    fieldsPage_->emitCompletenessCheck();
}

void QtUserSearchWindow::setNameSuggestions(const std::vector<std::string>& suggestions) {
    if (detailsPage_) {
        detailsPage_->setNameSuggestions(suggestions);
    }
}

void QtUserSearchWindow::prepopulateJIDAndName(const JID& jid, const std::string& name) {
    firstPage_->jid_->setText(P2QSTRING(jid.toBare().toString()));
    detailsPage_->setJID(jid);
    lastPage_ = 1;
    restart();
    next();
    detailsPage_->setName(name);
}

void QtUserSearchWindow::setContactSuggestions(const std::vector<Contact::ref>& suggestions) {
    if (type_ == Type::AddContact) {
        firstPage_->jid_->setSuggestions(suggestions);
    } else {
        firstMultiJIDPage_->jid_->setSuggestions(suggestions);
    }
}

void QtUserSearchWindow::setJIDs(const std::vector<JID> &jids) {
    for (auto&& jid : jids) {
        addSearchedJIDToList(std::make_shared<Contact>("", jid, StatusShow::None, ""));
    }
    onJIDUpdateRequested(jids);
}

void QtUserSearchWindow::setOriginator(const JID& originator) {
    originatorJID_ = originator;
}

void QtUserSearchWindow::setRoomJID(const JID& roomJID) {
    roomJID_ = roomJID;
}

std::string QtUserSearchWindow::getReason() const {
    return Q2PSTRING(firstMultiJIDPage_->reason_->text());
}

std::vector<JID> QtUserSearchWindow::getJIDs() const {
    std::vector<JID> jids;
    for (Contact::ref contact : contactVector_) {
        jids.push_back(contact->jid);
    }
    return jids;
}

void QtUserSearchWindow::setCanStartImpromptuChats(bool supportsImpromptu) {
    supportsImpromptu_ = supportsImpromptu;
    if (type_ == Type::ChatToContact) {
        firstMultiJIDPage_->contactList_->setMaximumNoOfContactsToOne(!supportsImpromptu_);
    }
}

void QtUserSearchWindow::updateContacts(const std::vector<Contact::ref>& contacts) {
    if (type_ != Type::AddContact) {
        firstMultiJIDPage_->contactList_->updateContacts(contacts);
    }
}

void QtUserSearchWindow::addContacts(const std::vector<Contact::ref>& contacts) {
    if (type_ != Type::AddContact) {
        /* prevent duplicate JIDs from appearing in the contact list */
        for (Contact::ref newContact : contacts) {
            bool found = false;
            for (Contact::ref oldContact : contactVector_) {
                if (newContact->jid == oldContact->jid) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                contactVector_.push_back(newContact);
            }
        }
        if (type_ != Type::InviteToChat && !supportsImpromptu_ && contactVector_.size() > 1) {
            contactVector_.resize(1); /* can't chat with more than one user */
        }
        firstMultiJIDPage_->contactList_->setList(contactVector_);
        firstMultiJIDPage_->emitCompletenessCheck();
        if (type_ == Type::ChatToContact) {
            firstMultiJIDPage_->groupBox->setEnabled(supportsImpromptu_ ? true : (contactVector_.size() < 1));
        }
    }
}

void QtUserSearchWindow::setCanSupplyDescription(bool allowed) {
    firstMultiJIDPage_->label->setVisible(allowed);
    firstMultiJIDPage_->reason_->setVisible(allowed);
}

void QtUserSearchWindow::handleAddViaSearch() {
    searchNext_ = true;
    next();
}

void QtUserSearchWindow::handleListChanged(std::vector<Contact::ref> list) {
    contactVector_ = list;
    if (type_ == Type::ChatToContact) {
        firstMultiJIDPage_->groupBox->setEnabled(supportsImpromptu_ ? 1 : (contactVector_.size() < 1));
    }
}

void QtUserSearchWindow::handleJIDsAdded(std::vector<JID> jids) {
    onJIDAddRequested(jids);
}

void QtUserSearchWindow::setResults(const std::vector<UserSearchResult>& results) {
    UserSearchModel *newModel = new UserSearchModel();
    newModel->setResults(results);
    resultsPage_->results_->setModel(newModel);
    resultsPage_->results_->setItemDelegate(delegate_);
    resultsPage_->results_->setHeaderHidden(true);
    delete model_;
    model_ = newModel;
    resultsPage_->setNoResults(model_->rowCount() == 0);
    resultsPage_->emitCompletenessCheck();
}

void QtUserSearchWindow::setResultsForm(Form::ref results) {
    QtFormResultItemModel *newModel = new QtFormResultItemModel(this);
    newModel->setForm(results);
    resultsPage_->results_->setModel(newModel);
    resultsPage_->results_->setItemDelegate(new QItemDelegate());
    resultsPage_->results_->setHeaderHidden(false);
#if QT_VERSION >= 0x050000
    resultsPage_->results_->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    resultsPage_->results_->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif
    delete model_;
    model_ = newModel;
    resultsPage_->setNoResults(model_->rowCount() == 0);
    resultsPage_->emitCompletenessCheck();
}

void QtUserSearchWindow::setSelectedService(const JID& jid) {
    myServer_ = jid;
}

void QtUserSearchWindow::handleJIDEditingDone() {
    onJIDEditFieldChanged(JID(Q2PSTRING(firstPage_->jid_->text())));
}

void QtUserSearchWindow::setFirstPage(QString title) {
    if (page(1) != nullptr) {
        removePage(1);
    }
    if (type_ == Type::AddContact) {
        firstPage_ = new QtUserSearchFirstPage(type_, title.isEmpty() ? firstPage_->title() : title, settings_);
        connect(firstPage_->jid_, SIGNAL(textEdited(QString)), this, SLOT(handleContactSuggestionRequested(QString)));
        connect(firstPage_->jid_, SIGNAL(textEdited(QString)), this, SLOT(handleJIDEditingDone()), Qt::UniqueConnection);
        firstPage_->jid_->onUserSelected.connect(boost::bind(&QtUserSearchWindow::handleOnSearchedJIDSelected, this, _1));
        connect(firstPage_->byJID_, SIGNAL(toggled(bool)), this, SLOT(handleFirstPageRadioChange()));
        connect(firstPage_->byLocalSearch_, SIGNAL(toggled(bool)), this, SLOT(handleFirstPageRadioChange()));
        connect(firstPage_->byRemoteSearch_, SIGNAL(toggled(bool)), this, SLOT(handleFirstPageRadioChange()));
#if QT_VERSION >= 0x040700
        firstPage_->jid_->setPlaceholderText(tr("alice@wonderland.lit"));
#endif
        firstPage_->service_->setEnabled(false);
        setPage(1, firstPage_);
    } else {
        firstMultiJIDPage_ = new QtUserSearchFirstMultiJIDPage(type_, title.isEmpty() ? firstMultiJIDPage_->title() : title, settings_);
        connect(firstMultiJIDPage_->addContactButton_, SIGNAL(clicked()), this, SLOT(addContact()));
        connect(firstMultiJIDPage_->jid_, SIGNAL(textEdited(QString)), this, SLOT(handleContactSuggestionRequested(QString)));
        firstMultiJIDPage_->jid_->onUserSelected.connect(boost::bind(&QtUserSearchWindow::addSearchedJIDToList, this, _1));
        connect(firstMultiJIDPage_->addViaSearchButton_, SIGNAL(clicked()), this, SLOT(handleAddViaSearch()));
        connect(firstMultiJIDPage_->contactList_, SIGNAL(onListChanged(std::vector<Contact::ref>)), this, SLOT(handleListChanged(std::vector<Contact::ref>)));
        connect(firstMultiJIDPage_->contactList_, SIGNAL(onJIDsAdded(std::vector<JID>)), this, SLOT(handleJIDsAdded(std::vector<JID>)));
        connect(firstMultiJIDPage_, SIGNAL(onJIDsDropped(std::vector<JID>)), this, SLOT(handleJIDsAdded(std::vector<JID>)));
        setPage(1, firstMultiJIDPage_);
    }
}

void QtUserSearchWindow::setSecondPage() {
    if (page(2) != nullptr) {
        removePage(2);
    }
    fieldsPage_ = new QtUserSearchFieldsPage();
    fieldsPage_->fetchingThrobber_->setMovie(new QMovie(":/icons/throbber.gif", QByteArray(), this));
    fieldsPage_->fetchingThrobber_->movie()->stop();
    setPage(2, fieldsPage_);
}

void QtUserSearchWindow::setThirdPage() {
    if (page(3) != nullptr) {
        removePage(3);
    }
    resultsPage_ = new QtUserSearchResultsPage();

#ifdef SWIFT_PLATFORM_MACOSX
    resultsPage_->results_->setAlternatingRowColors(true);
#endif
    if (type_ == Type::AddContact) {
        connect(resultsPage_, SIGNAL(onUserTriggersContinue()), this, SLOT(next()));
    }
    else {
        connect(resultsPage_, SIGNAL(onUserTriggersContinue()), this, SLOT(next()));
    }
    setPage(3, resultsPage_);
}

void QtUserSearchWindow::clearForm() {
    fieldsPage_->fetchingThrobber_->show();
    fieldsPage_->fetchingThrobber_->movie()->start();
    fieldsPage_->fetchingLabel_->show();
    QWidget* legacySearchWidgets[8] = {fieldsPage_->nickInputLabel_, fieldsPage_->nickInput_, fieldsPage_->firstInputLabel_, fieldsPage_->firstInput_, fieldsPage_->lastInputLabel_, fieldsPage_->lastInput_, fieldsPage_->emailInputLabel_, fieldsPage_->emailInput_};
    for (auto&& legacySearchWidget : legacySearchWidgets) {
        legacySearchWidget->hide();
        if (QLineEdit* edit = qobject_cast<QLineEdit*>(legacySearchWidget)) {
            edit->clear();
        }
    }
    fieldsPage_->emitCompletenessCheck();
}

void QtUserSearchWindow::clear() {
    QString howText;
    if (type_ == Type::AddContact) {
        firstPage_->errorLabel_->setVisible(false);
        howText = QString(tr("How would you like to find the user to add?"));
        firstPage_->howLabel_->setText(howText);
        firstPage_->byJID_->setChecked(true);
        handleFirstPageRadioChange();
    } else {
        contactVector_.clear();
        firstMultiJIDPage_->contactList_->setList(contactVector_);
        firstMultiJIDPage_->errorLabel_->setVisible(false);
        if (type_ == Type::ChatToContact) {
            howText = QString(tr("List of participants:"));
        } else if (type_ == Type::InviteToChat) {
            howText = QString(tr("Who do you want to invite to the chat?"));
        }
        firstMultiJIDPage_->howLabel_->setText(howText);
    }
    clearForm();
    resultsPage_->results_->setModel(nullptr);
    delete model_;
    model_ = nullptr;
    restart();
    lastPage_ = 1;
}

void QtUserSearchWindow::setError(const QString& error) {
    if (error.isEmpty()) {
        if (type_ == Type::AddContact) {
            firstPage_->errorLabel_->hide();
        } else {
            firstMultiJIDPage_->errorLabel_->hide();
        }
    }
    else {
        if (type_ == Type::AddContact) {
            firstPage_->errorLabel_->setText(QString("<font color='red'>%1</font>").arg(error));
            firstPage_->errorLabel_->show();
        } else {
            firstMultiJIDPage_->errorLabel_->setText(QString("<font color='red'>%1</font>").arg(error));
            firstMultiJIDPage_->errorLabel_->show();
        }
        restart();
        lastPage_ = 1;
    }
}

void QtUserSearchWindow::setSearchError(bool error) {
    if (error) {
        setError(tr("Error while searching"));
    }
}

void QtUserSearchWindow::setServerSupportsSearch(bool support) {
    if (!support) {
        setError(tr("This server doesn't support searching for users."));
    }
}

}
