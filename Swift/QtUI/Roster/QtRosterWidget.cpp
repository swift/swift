/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/Roster/QtRosterWidget.h>

#include <QContextMenuEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>

#include <Swift/Controllers/Roster/ContactRosterItem.h>
#include <Swift/Controllers/Roster/GroupRosterItem.h>
#include <Swift/Controllers/UIEvents/RemoveRosterItemUIEvent.h>
#include <Swift/Controllers/UIEvents/RenameGroupUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestChangeBlockStateUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestContactEditorUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestWhiteboardUIEvent.h>
#include <Swift/Controllers/UIEvents/SendFileUIEvent.h>
#include <Swift/Controllers/UIEvents/ShowProfileForRosterItemUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>

#include <Swift/QtUI/QtContactEditWindow.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtRosterWidget::QtRosterWidget(UIEventStream* eventStream, SettingsProvider* settings, QWidget* parent) : QtTreeWidget(eventStream, settings, MessageDefaultJID, parent) {

}

QtRosterWidget::~QtRosterWidget() {

}

void QtRosterWidget::handleEditUserActionTriggered(bool /*checked*/) {
    QModelIndexList selectedIndexList = getSelectedIndexes();
    if (selectedIndexList.empty()) {
        return;
    }
    QModelIndex index = selectedIndexList[0];
    if (!index.isValid()) {
        return;
    }
    RosterItem* item = static_cast<RosterItem*>(index.internalPointer());
    if (ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item)) {
        eventStream_->send(std::make_shared<RequestContactEditorUIEvent>(contact->getJID()));
    }
}

void QtRosterWidget::contextMenuEvent(QContextMenuEvent* event) {
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid()) {
        return;
    }
    RosterItem* item = static_cast<RosterItem*>(index.internalPointer());
    QMenu contextMenu;
    if (ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item)) {
        QAction* editContact = contextMenu.addAction(tr("Edit…"));
        editContact->setEnabled(isOnline());
        QAction* removeContact = contextMenu.addAction(tr("Remove"));
        removeContact->setEnabled(isOnline());
        QAction* showProfileForContact = contextMenu.addAction(tr("Show Profile"));

        QAction* unblockContact = nullptr;
        if (contact->blockState() == ContactRosterItem::IsBlocked ||
            contact->blockState() == ContactRosterItem::IsDomainBlocked) {
            unblockContact = contextMenu.addAction(tr("Unblock"));
            unblockContact->setEnabled(isOnline());
        }

        QAction* blockContact = nullptr;
        if (contact->blockState() == ContactRosterItem::IsUnblocked) {
            blockContact = contextMenu.addAction(tr("Block"));
            blockContact->setEnabled(isOnline());
        }

#ifdef SWIFT_EXPERIMENTAL_FT
        QAction* sendFile = nullptr;
        if (contact->supportsFeature(ContactRosterItem::FileTransferFeature)) {
            sendFile = contextMenu.addAction(tr("Send File"));
            sendFile->setEnabled(isOnline());
        }
#endif
#ifdef SWIFT_EXPERIMENTAL_WB
        QAction* startWhiteboardChat = nullptr;
        if (contact->supportsFeature(ContactRosterItem::WhiteboardFeature)) {
            startWhiteboardChat = contextMenu.addAction(tr("Start Whiteboard Chat"));
            startWhiteboardChat->setEnabled(isOnline());
        }
#endif
        QAction* result = contextMenu.exec(event->globalPos());
        if (result == editContact) {
            eventStream_->send(std::make_shared<RequestContactEditorUIEvent>(contact->getJID()));
        }
        else if (result == removeContact) {
            if (QtContactEditWindow::confirmContactDeletion(contact->getJID())) {
                eventStream_->send(std::make_shared<RemoveRosterItemUIEvent>(contact->getJID()));
            }
        }
        else if (result == showProfileForContact) {
            eventStream_->send(std::make_shared<ShowProfileForRosterItemUIEvent>(contact->getJID()));
        }
        else if (unblockContact && result == unblockContact) {
            if (contact->blockState() == ContactRosterItem::IsDomainBlocked) {
                QMessageBox messageBox(QMessageBox::Question, tr("Swift"), tr("%2 is currently blocked because of a block on all users of the %1 service.\n %2 cannot be unblocked individually; do you want to unblock all %1 users?").arg(P2QSTRING(contact->getJID().getDomain()), P2QSTRING(contact->getJID().toString())), QMessageBox::NoButton, this);
                QPushButton* unblockDomainButton = messageBox.addButton(tr("Unblock %1 domain").arg(P2QSTRING(contact->getJID().getDomain())), QMessageBox::AcceptRole);
                messageBox.addButton(QMessageBox::Abort);

                messageBox.exec();
                if (messageBox.clickedButton() == unblockDomainButton)  {
                    eventStream_->send(std::make_shared<RequestChangeBlockStateUIEvent>(RequestChangeBlockStateUIEvent::Unblocked, contact->getJID().getDomain()));
                }
            } else {
                eventStream_->send(std::make_shared<RequestChangeBlockStateUIEvent>(RequestChangeBlockStateUIEvent::Unblocked, contact->getJID()));
            }
        }
        else if (blockContact && result == blockContact) {
            eventStream_->send(std::make_shared<RequestChangeBlockStateUIEvent>(RequestChangeBlockStateUIEvent::Blocked, contact->getJID()));
        }
#ifdef SWIFT_EXPERIMENTAL_FT
        else if (sendFile && result == sendFile) {
            QString fileName = QFileDialog::getOpenFileName(this, tr("Send File"), "", tr("All Files (*);;"));
            if (!fileName.isEmpty()) {
                eventStream_->send(std::make_shared<SendFileUIEvent>(contact->getJID(), Q2PSTRING(fileName)));
            }
        }
#endif
#ifdef SWIFT_EXPERIMENTAL_WB
        else if (startWhiteboardChat && result == startWhiteboardChat) {
            eventStream_->send(std::make_shared<RequestWhiteboardUIEvent>(contact->getJID()));
        }
#endif
    }
    else if (GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item)) {
        QAction* renameGroupAction = contextMenu.addAction(tr("Rename"));
        if (P2QSTRING(group->getDisplayName()) == tr("Contacts")) {
            renameGroupAction->setEnabled(false);
        }
        else {
            renameGroupAction->setEnabled(isOnline());
        }
        QAction* result = contextMenu.exec(event->globalPos());
        if (result == renameGroupAction) {
            renameGroup(group);
        }
    }
}

void QtRosterWidget::renameGroup(GroupRosterItem* group) {
    bool ok;
    QString newName = QInputDialog::getText(nullptr, tr("Rename group"), tr("Enter a new name for group '%1':").arg(P2QSTRING(group->getDisplayName())), QLineEdit::Normal, P2QSTRING(group->getDisplayName()), &ok);
    if (ok) {
        eventStream_->send(std::make_shared<RenameGroupUIEvent>(group->getDisplayName(), Q2PSTRING(newName)));
    }
}

}
