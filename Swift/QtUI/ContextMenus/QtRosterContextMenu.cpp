/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/ContextMenus/QtRosterContextMenu.h"

#include <QInputDialog>
#include <QLineEdit>
#include <QMenu>
#include <QDebug>
#include <QDialog>

#include <boost/shared_ptr.hpp>

#include "Swiften/Roster/ContactRosterItem.h"
#include "Swiften/Roster/GroupRosterItem.h"
#include "Swiften/Base/String.h"
#include "Swiften/Roster/Roster.h"
#include "Swift/Controllers/UIEvents/UIEvent.h"
#include "Swift/Controllers/UIEvents/RemoveRosterItemUIEvent.h"
#include "Swift/Controllers/UIEvents/RenameRosterItemUIEvent.h"
#include "Swift/QtUI/QtSwiftUtil.h"
#include "Swift/QtUI/QtSetGroupsDialog.h"


namespace Swift {

QtRosterContextMenu::QtRosterContextMenu(UIEventStream* eventStream, QtTreeWidget* treeWidget) {
	eventStream_ = eventStream;
	treeWidget_ = treeWidget;
}

void QtRosterContextMenu::show(RosterItem* item) {
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
	if (!contact) {
		return;
	}
	item_ = item;
	QMenu contextMenu;
	contextMenu.addAction("Remove", this, SLOT(handleRemoveContact()));
	contextMenu.addAction("Rename", this, SLOT(handleRenameContact()));
	contextMenu.addAction("Groups", this, SLOT(handleRegroupContact()));
	/*QMenu* groupsMenu = contextMenu.addMenu("Groups");
	std::map<QAction, String> groupActions;
	for (int i = 0; i < 0; i++) {
		String groupName;
		groupActions[groupsMenu->addAction(P2QSTRING(groupName))] = groupName;
	}
	groupsMenu->addSeparator();
	groupsMenu->addAction("New Group", this SLOT(handleNewGroup()));*/
	contextMenu.exec(QCursor::pos());
}

void QtRosterContextMenu::handleRegroupContact() {
	QList<QString> allGroups;
	foreach (RosterItem* item, treeWidget_->getRoster()->getRoot()->getChildren()) {
		GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item);
		if (group) {
			allGroups.push_back(P2QSTRING(group->getDisplayName()));
		}
	}
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item_);
	assert(contact);
	QtSetGroupsDialog groupDialog(contact, allGroups);

	if (groupDialog.exec() == QDialog::Accepted) {
		eventStream_->send(groupDialog.getRegroupEvent());
	}
}

void QtRosterContextMenu::handleRemoveContact() {
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item_);
	assert(contact);
	eventStream_->send(boost::shared_ptr<UIEvent>(new RemoveRosterItemUIEvent(contact->getJID())));
}

void QtRosterContextMenu::handleRenameContact() {
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item_);
	assert(contact);
	bool ok;
	QString newName = QInputDialog::getText(NULL, "Rename contact", "New name for " + P2QSTRING(item_->getDisplayName()), QLineEdit::Normal, P2QSTRING(item_->getDisplayName()), &ok);
	if (ok) {
		eventStream_->send(boost::shared_ptr<UIEvent>(new RenameRosterItemUIEvent(contact->getJID(), Q2PSTRING(newName))));
	}
}

}
