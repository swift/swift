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

#include <boost/shared_ptr.hpp>

#include "Swiften/Roster/ContactRosterItem.h"
#include "Swiften/Base/String.h"
#include "Swift/Controllers/UIEvents/UIEvent.h"
#include "Swift/Controllers/UIEvents/RemoveRosterItemUIEvent.h"
#include "Swift/Controllers/UIEvents/RenameRosterItemUIEvent.h"
#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {

QtRosterContextMenu::QtRosterContextMenu(UIEventStream* eventStream) {
	eventStream_ = eventStream;
}

void QtRosterContextMenu::show(RosterItem* item) {
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
	if (!contact) {
		return;
	}
	item_ = item;
	QMenu* contextMenu = new QMenu();
	contextMenu->addAction("Remove", this, SLOT(handleRemoveContact()));
	contextMenu->addAction("Rename", this, SLOT(handleRenameContact()));
	contextMenu->exec(QCursor::pos());
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
