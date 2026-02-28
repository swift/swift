/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#include <Swift/QtUI/Roster/QtOccupantListWidget.h>

#include <QAction>
#include <QContextMenuEvent>
#include <QInputDialog>
#include <QMenu>

#include <Swift/Controllers/Roster/ContactRosterItem.h>
#include <Swift/Controllers/Roster/GroupRosterItem.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtOccupantListWidget::QtOccupantListWidget(UIEventStream* eventStream, SettingsProvider* settings, MessageTarget privateMessageTarget, QWidget* parent) : QtTreeWidget(eventStream, settings, privateMessageTarget, parent) {

}

QtOccupantListWidget::~QtOccupantListWidget() {

}

void QtOccupantListWidget::setAvailableOccupantActions(const std::vector<ChatWindow::OccupantAction>& actions) {
    availableOccupantActions_ = actions;
}

void QtOccupantListWidget::contextMenuEvent(QContextMenuEvent* event) {
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid()) {
        return;
    }

    RosterItem* item = static_cast<RosterItem*>(index.internalPointer());
    ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
    if (contact) {
        onSomethingSelectedChanged(contact);
        QMenu contextMenu;
        if (availableOccupantActions_.empty()) {
            QAction* noAction = contextMenu.addAction(tr("No actions for this user"));
            noAction->setEnabled(false);
            contextMenu.exec(event->globalPos());
        }
        else {
            std::map<QAction*, ChatWindow::OccupantAction> actions;
            for (const auto& availableAction : availableOccupantActions_) {
                QString text = "Error: missing string";
                switch (availableAction) {
                    case ChatWindow::Kick: text = tr("Kick user"); break;
                    case ChatWindow::Ban: text = tr("Kick and ban user"); break;
                    case ChatWindow::MakeModerator: text = tr("Make moderator"); break;
                    case ChatWindow::MakeParticipant: text = tr("Make participant"); break;
                    case ChatWindow::MakeVisitor: text = tr("Remove voice"); break;
                    case ChatWindow::AddContact: text = tr("Add to contacts"); break;
                    case ChatWindow::ShowProfile: text = tr("Show profile"); break;
                }
                QAction* action = contextMenu.addAction(text);
                actions[action] = availableAction;
            }
            QAction* result = contextMenu.exec(event->globalPos());
            if (result) {
                onOccupantActionSelected(actions[result], contact);
            }
        }
    }
}

}

