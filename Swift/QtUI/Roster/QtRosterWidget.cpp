/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Roster/QtRosterWidget.h"

#include <QContextMenuEvent>
#include <QMenu>
#include <QInputDialog>

#include "Swift/Controllers/UIEvents/RequestContactEditorUIEvent.h"
#include "Swift/Controllers/UIEvents/RemoveRosterItemUIEvent.h"
#include "Swift/Controllers/UIEvents/RenameGroupUIEvent.h"
#include "QtContactEditWindow.h"
#include "Swift/Controllers/Roster/ContactRosterItem.h"
#include "Swift/Controllers/Roster/GroupRosterItem.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "QtSwiftUtil.h"

namespace Swift {

QtRosterWidget::QtRosterWidget(UIEventStream* eventStream, QWidget* parent) : QtTreeWidget(eventStream, parent) {

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
		eventStream_->send(boost::make_shared<RequestContactEditorUIEvent>(contact->getJID()));
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
		QAction* editContact = contextMenu.addAction(tr("Edit"));
		QAction* removeContact = contextMenu.addAction(tr("Remove"));
		QAction* result = contextMenu.exec(event->globalPos());
		if (result == editContact) {
			eventStream_->send(boost::make_shared<RequestContactEditorUIEvent>(contact->getJID()));
		}
		else if (result == removeContact) {
			if (QtContactEditWindow::confirmContactDeletion(contact->getJID())) {
				eventStream_->send(boost::make_shared<RemoveRosterItemUIEvent>(contact->getJID()));
			}
		}
	}
	else if (GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item)) {
		QAction* renameGroupAction = contextMenu.addAction(tr("Rename"));
		QAction* result = contextMenu.exec(event->globalPos());
		if (result == renameGroupAction) {
			renameGroup(group);
		}
	}
}

void QtRosterWidget::renameGroup(GroupRosterItem* group) {
	bool ok;
	QString newName = QInputDialog::getText(NULL, tr("Rename group"), tr("Enter a new name for group '%1':").arg(P2QSTRING(group->getDisplayName())), QLineEdit::Normal, P2QSTRING(group->getDisplayName()), &ok);
	if (ok) {
		eventStream_->send(boost::make_shared<RenameGroupUIEvent>(group->getDisplayName(), Q2PSTRING(newName)));
	}
}

void QtRosterWidget::currentChanged(const QModelIndex& current, const QModelIndex& previous) {
	bool valid = false;
	QModelIndexList selectedIndexList = getSelectedIndexes();
	if (selectedIndexList.empty() || !selectedIndexList[0].isValid()) {
		/* I didn't quite understand why using current didn't seem to work here.*/
	}
	else if (current.isValid()) {
		RosterItem* item = static_cast<RosterItem*>(current.internalPointer());
		if (dynamic_cast<ContactRosterItem*>(item)) {
			valid = true;
		}
	}
	onSomethingSelectedChanged(valid);
	QTreeView::currentChanged(current, previous);
}

}
