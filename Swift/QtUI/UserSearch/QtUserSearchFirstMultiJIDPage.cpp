/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/UserSearch/QtUserSearchFirstMultiJIDPage.h>

#include <QMessageBox>
#include <QMimeData>
#include <QUrl>

#include <Swift/Controllers/Settings/SettingsProvider.h>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/UserSearch/QtContactListWidget.h>
#include <Swift/QtUI/UserSearch/QtSuggestingJIDInput.h>

namespace Swift {

QtUserSearchFirstMultiJIDPage::QtUserSearchFirstMultiJIDPage(UserSearchWindow::Type type, const QString& title, SettingsProvider* settings) {
    setupUi(this);
    setTitle(title);
    QString introText = "";
    switch (type) {
        case UserSearchWindow::Type::AddContact:
            introText = tr("Add another user to your contact list");
            break;
        case UserSearchWindow::Type::ChatToContact:
            introText = tr("Chat to another user");
            break;
        case UserSearchWindow::Type::InviteToChat:
            introText = tr("Invite contact to chat");
            break;
    }

    setSubTitle(QString(tr("%1. If you know their address you can enter it directly, or you can search for them.")).arg(introText));

    contactList_ = new QtContactListWidget(this, settings);
    horizontalLayout_5->addWidget(contactList_);

    jid_ = new QtSuggestingJIDInput(this, settings);
    horizontalLayout_6->insertWidget(0, jid_);

    connect(contactList_, SIGNAL(onListChanged(std::vector<Contact::ref>)), this, SLOT(emitCompletenessCheck()));
    connect(jid_, SIGNAL(editingDone()), this, SLOT(handleEditingDone()));

    setAcceptDrops(true);
}

bool QtUserSearchFirstMultiJIDPage::isComplete() const {
    return !contactList_->getList().empty();
}

void QtUserSearchFirstMultiJIDPage::reset() {
    jid_->clear();
    reason_->clear();
}

void QtUserSearchFirstMultiJIDPage::emitCompletenessCheck() {
    emit completeChanged();
}

void QtUserSearchFirstMultiJIDPage::handleEditingDone() {
    addContactButton_->setFocus();
}

void QtUserSearchFirstMultiJIDPage::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/vnd.swift.contact-jid-list")
        || event->mimeData()->hasFormat("application/vnd.swift.contact-jid-muc")) {
            if (!contactList_->isFull()) {
                event->acceptProposedAction();
            }
    }
}

void QtUserSearchFirstMultiJIDPage::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasFormat("application/vnd.swift.contact-jid-list")) {
        QByteArray dataBytes = event->mimeData()->data("application/vnd.swift.contact-jid-list");
        QDataStream dataStream(&dataBytes, QIODevice::ReadOnly);
        std::vector<JID> jids;
        while (!dataStream.atEnd()) {
            QString jidString;
            dataStream >> jidString;
            jids.push_back(Q2PSTRING(jidString));
        }
        onJIDsDropped(jids);
    } else if (event->mimeData()->hasFormat("application/vnd.swift.contact-jid-muc")) {
        QMessageBox* messageBox = new QMessageBox(this);
        messageBox->setText(tr("You can't invite a room to chat."));
        messageBox->setWindowTitle(tr("Error inviting room to chat"));
        messageBox->show();
    }
}

}
