/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */


#include "Roster/QtOccupantListWidget.h"

#include <QContextMenuEvent>
#include <QMenu>
#include <QInputDialog>

#include "Swift/Controllers/Roster/ContactRosterItem.h"
#include "Swift/Controllers/Roster/GroupRosterItem.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "QtSwiftUtil.h"

namespace Swift {

QtOccupantListWidget::QtOccupantListWidget(UIEventStream* eventStream, QWidget* parent) : QtTreeWidget(eventStream, parent) {

}

QtOccupantListWidget::~QtOccupantListWidget() {

}

void QtOccupantListWidget::contextMenuEvent(QContextMenuEvent* event) {
//	QModelIndex index = indexAt(event->pos());
//	if (!index.isValid()) {
//		return;
//	}
//	RosterItem* item = static_cast<RosterItem*>(index.internalPointer());
//	QMenu contextMenu;
//	if (ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item)) {
//		QAction* editContact = contextMenu.addAction(tr("Edit"));
//		QAction* removeContact = contextMenu.addAction(tr("Remove"));
//		QAction* result = contextMenu.exec(event->globalPos());
//		if (result == editContact) {
//			eventStream_->send(boost::make_shared<RequestContactEditorUIEvent>(contact->getJID()));
//		}
//		else if (result == removeContact) {
//			if (QtContactEditWindow::confirmContactDeletion(contact->getJID())) {
//				eventStream_->send(boost::make_shared<RemoveRosterItemUIEvent>(contact->getJID()));
//			}
//		}
//	}
//	else if (GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item)) {
//		QAction* renameGroupAction = contextMenu.addAction(tr("Rename"));
//		QAction* result = contextMenu.exec(event->globalPos());
//		if (result == renameGroupAction) {
//			renameGroup(group);
//		}
//	}
}

}

