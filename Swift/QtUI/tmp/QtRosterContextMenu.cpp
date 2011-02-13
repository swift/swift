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
#include <QMessageBox>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include "Swiften/Roster/ContactRosterItem.h"
#include "Swiften/Roster/GroupRosterItem.h"
#include "Swiften/Base/String.h"
#include "Swiften/Roster/Roster.h"
#include "Swift/Controllers/UIEvents/UIEvent.h"
#include "Swift/Controllers/UIEvents/RemoveRosterItemUIEvent.h"
#include "Swift/Controllers/UIEvents/RenameRosterItemUIEvent.h"
#include "Swift/QtUI/QtSwiftUtil.h"
#include "Swift/QtUI/QtEditContactDialog.h"


namespace Swift {

QtRosterContextMenu::QtRosterContextMenu(UIEventStream* eventStream, QtTreeWidget* treeWidget) : eventStream_(eventStream), treeWidget_(treeWidget), item_(NULL) {
}

void QtRosterContextMenu::show(RosterItem* item) {
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
	item_ = item;
	QMenu contextMenu;
	if (contact) {
		contextMenu.addAction("Edit", this, SLOT(handleEditContact()));
	}
	GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item);
	if (group) {
		contextMenu.addAction("Rename", this, SLOT(handleRenameGroup()));
	}
	contextMenu.exec(QCursor::pos());
}

void QtRosterContextMenu::handleEditContact() {
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item_);
	assert(contact);

	// Figure out all the groups the contact is in
	QList<QString> allGroups;
	foreach (RosterItem* item, treeWidget_->getRoster()->getRoot()->getChildren()) {
		GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item);
		if (group) {
			allGroups.push_back(P2QSTRING(group->getDisplayName()));
		}
	}

	QtEditContactDialog editDialog(contact, allGroups, eventStream_);

	if (groupDialog.exec() == QDialog::Accepted) {
		eventStream_->send(groupDialog.getRegroupEvent());
	}

	/*	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item_);
	QMessageBox msgBox;
	msgBox.setWindowTitle("Confirm contact deletion");
	msgBox.setText("Are you sure you want to delete this contact?");
	msgBox.setInformativeText(QString("This will remove the contact '%1' from all groups they may be in.").arg(P2QSTRING(contact->getJID().toString())));
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::Yes);
	int ret = msgBox.exec();
	if (ret == QMessageBox::Yes) {
		eventStream_->send(boost::shared_ptr<UIEvent>(new RemoveRosterItemUIEvent(contact->getJID())));
	}*/


/*	bool ok;
	QString newName = QInputDialog::getText(NULL, "Rename contact", "New name for " + P2QSTRING(item_->getDisplayName()), QLineEdit::Normal, P2QSTRING(item_->getDisplayName()), &ok);
	if (ok) {
		eventStream_->send(boost::shared_ptr<UIEvent>(new RenameRosterItemUIEvent(contact->getJID(), Q2PSTRING(newName))));
	}*/
}

void QtRosterContextMenu::handleRenameGroup() {
	/*
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
			eventStream_->send(boost::make_shared<RegroupRosterItemUIEvent>(contact->getJID(), addedGroups, removedGroups));
		}
	}
	*/
}

}
