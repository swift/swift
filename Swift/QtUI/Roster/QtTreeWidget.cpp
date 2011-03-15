/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Roster/QtTreeWidget.h"

#include <QMenu>
#include <QContextMenuEvent>
#include <QInputDialog>
#include <boost/smart_ptr/make_shared.hpp>

#include "Swiften/Base/Platform.h"
#include "Swift/Controllers/Roster/ContactRosterItem.h"
#include "Swift/Controllers/Roster/GroupRosterItem.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UIEvents/RequestChatUIEvent.h"
#include "Swift/Controllers/UIEvents/RequestContactEditorUIEvent.h"
#include "Swift/Controllers/UIEvents/RemoveRosterItemUIEvent.h"
#include "Swift/Controllers/UIEvents/RenameGroupUIEvent.h"
#include "QtSwiftUtil.h"
#include "QtContactEditWindow.h"

namespace Swift {

QtTreeWidget::QtTreeWidget(UIEventStream* eventStream, QWidget* parent) : QTreeView(parent), editable_(false) {
	eventStream_ = eventStream;
	model_ = new RosterModel(this);
	setModel(model_);
	delegate_ = new RosterDelegate(this);
	setItemDelegate(delegate_);
	setHeaderHidden(true);
#ifdef SWIFT_PLATFORM_MACOSX
	setAlternatingRowColors(true);
#endif
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	expandAll();
	setAnimated(true);
	setIndentation(0);
	setRootIsDecorated(true);
	connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(handleItemActivated(const QModelIndex&)));
	connect(model_, SIGNAL(itemExpanded(const QModelIndex&, bool)), this, SLOT(handleModelItemExpanded(const QModelIndex&, bool)));
	connect(this, SIGNAL(expanded(const QModelIndex&)), this, SLOT(handleExpanded(const QModelIndex&)));
	connect(this, SIGNAL(collapsed(const QModelIndex&)), this, SLOT(handleCollapsed(const QModelIndex&)));
	connect(this, SIGNAL(clicked(const QModelIndex&)), this, SLOT(handleClicked(const QModelIndex&)));
}

QtTreeWidget::~QtTreeWidget() {
	delete model_;
	delete delegate_;
}

void QtTreeWidget::setRosterModel(Roster* roster) {
	roster_ = roster;
	model_->setRoster(roster);
	expandAll();
}

QtTreeWidgetItem* QtTreeWidget::getRoot() {
	return treeRoot_;
}

void QtTreeWidget::handleClicked(const QModelIndex& index) {
	GroupRosterItem* item = dynamic_cast<GroupRosterItem*>(static_cast<RosterItem*>(index.internalPointer()));
	if (item) {
		setExpanded(index, !isExpanded(index));
	}
	currentChanged(index, QModelIndex());
}

QModelIndexList QtTreeWidget::getSelectedIndexes() const {
	// Not using selectedIndexes(), because this seems to cause a crash in Qt (4.7.0) in the
	// QModelIndexList destructor.
	// This is a workaround posted in http://www.qtcentre.org/threads/16933 (although this case
	// was resolved by linking against the debug libs, ours isn't, and we're not alone)
	QItemSelection ranges = selectionModel()->selection();
	QModelIndexList selectedIndexList;
	for (int i = 0; i < ranges.count(); ++i) {
		QModelIndex parent = ranges.at(i).parent();
		int right = ranges.at(i).model()->columnCount(parent) - 1;
		if (ranges.at(i).left() == 0 && ranges.at(i).right() == right) {
			for (int r = ranges.at(i).top(); r <= ranges.at(i).bottom(); ++r) {
				selectedIndexList.append(ranges.at(i).model()->index(r, 0, parent));
			}
		}
	}
	return selectedIndexList;
}

void QtTreeWidget::currentChanged(const QModelIndex& current, const QModelIndex& previous) {
	bool valid = false;
	QModelIndexList selectedIndexList = getSelectedIndexes();
	if (!editable_ || selectedIndexList.empty() || !selectedIndexList[0].isValid()) {
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

void QtTreeWidget::handleItemActivated(const QModelIndex& index) {
	RosterItem* item = static_cast<RosterItem*>(index.internalPointer());
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
	if (contact) {
		eventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(contact->getJID())));
	}
}

void QtTreeWidget::handleEditUserActionTriggered(bool /*checked*/) {
	if (!editable_) {
		return;
	}
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

void QtTreeWidget::contextMenuEvent(QContextMenuEvent* event) {
	if (!editable_) {
		return;
	}
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

void QtTreeWidget::renameGroup(GroupRosterItem* group) {
	bool ok;
	QString newName = QInputDialog::getText(NULL, tr("Rename group"), tr("Enter a new name for group '%1':").arg(P2QSTRING(group->getDisplayName())), QLineEdit::Normal, P2QSTRING(group->getDisplayName()), &ok);
	if (ok) {
		eventStream_->send(boost::make_shared<RenameGroupUIEvent>(group->getDisplayName(), Q2PSTRING(newName)));
	}
}

void QtTreeWidget::handleExpanded(const QModelIndex& index) {
	GroupRosterItem* item = dynamic_cast<GroupRosterItem*>(static_cast<RosterItem*>(index.internalPointer()));
	if (item) {
		item->setExpanded(true);
	}
}

void QtTreeWidget::handleCollapsed(const QModelIndex& index) {
	GroupRosterItem* item = dynamic_cast<GroupRosterItem*>(static_cast<RosterItem*>(index.internalPointer()));
	if (item) {
		item->setExpanded(false);
	}
}

void QtTreeWidget::handleModelItemExpanded(const QModelIndex& index, bool shouldExpand) {
	if (!index.isValid()) {
		return;
	}
	bool alreadyRight = this->isExpanded(index) == shouldExpand;
	if (alreadyRight) {
		return;
	}
	setExpanded(index, shouldExpand);
}

void QtTreeWidget::drawBranches(QPainter*, const QRect&, const QModelIndex&) const {
}

void QtTreeWidget::show() {
	QWidget::show();
}

}
