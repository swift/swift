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
	item_ = item;
	QMenu contextMenu;
	if (contact) {
		contextMenu.addAction("Remove", this, SLOT(handleRemoveContact()));
		contextMenu.addAction("Rename", this, SLOT(handleRenameContact()));
		contextMenu.addAction("Groups", this, SLOT(handleRegroupContact()));
	}
	GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item);
	if (group) {
		contextMenu.addAction("Rename", this, SLOT(handleRenameGroup()));
	}
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

void QtRosterContextMenu::handleRenameGroup() {
	GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item_);
	assert(group);
	bool ok;
	QString newName = QInputDialog::getText(NULL, "Rename group", "New name for " + P2QSTRING(item_->getDisplayName()), QLineEdit::Normal, P2QSTRING(item_->getDisplayName()), &ok);
	if (ok) {
		std::vector<String> addedGroups;
		std::vector<String> removedGroups;
		addedGroups.push_back(Q2PSTRING(newName));
		removedGroups.push_back(group->getDisplayName());
		foreach (RosterItem* child, group->getChildren()) {
			ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(child);
			assert(contact);
			boost::shared_ptr<RegroupRosterItemUIEvent> regroupItem(new RegroupRosterItemUIEvent(contact->getJID(), addedGroups, removedGroups));
			eventStream_->send(regroupItem);
		}
	}
}

}
