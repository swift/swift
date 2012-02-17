/*
 * Copyright (c) 2011-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */


#include "Roster/QtOccupantListWidget.h"

#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QInputDialog>

#include "Swift/Controllers/Roster/ContactRosterItem.h"
#include "Swift/Controllers/Roster/GroupRosterItem.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "QtSwiftUtil.h"

namespace Swift {

QtOccupantListWidget::QtOccupantListWidget(UIEventStream* eventStream, SettingsProvider* settings, QWidget* parent) : QtTreeWidget(eventStream, settings, parent) {

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
			foreach (ChatWindow::OccupantAction availableAction, availableOccupantActions_) {
				QString text = "Error: missing string";
				switch (availableAction) {
					case ChatWindow::Kick: text = tr("Kick user"); break;
					case ChatWindow::Ban: text = tr("Kick and ban user"); break;
					case ChatWindow::MakeModerator: text = tr("Make moderator"); break;
					case ChatWindow::MakeParticipant: text = tr("Make participant"); break;
					case ChatWindow::MakeVisitor: text = tr("Remove voice"); break;
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

