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
#include "Swift/Controllers/UIEvents/RenameGroupUIEvent.h"
#include "QtSwiftUtil.h"

namespace Swift {

QtTreeWidget::QtTreeWidget(UIEventStream* eventStream, QWidget* parent) : QTreeView(parent) {
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
}


void QtTreeWidget::handleItemActivated(const QModelIndex& index) {
	RosterItem* item = static_cast<RosterItem*>(index.internalPointer());
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
	if (contact) {
		eventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(contact->getJID())));
	}
}

void QtTreeWidget::contextMenuEvent(QContextMenuEvent* event) {
	QModelIndex index = indexAt(event->pos());
	if (!index.isValid()) {
		return;
	}
	RosterItem* item = static_cast<RosterItem*>(index.internalPointer());
	QMenu contextMenu;
	if (ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item)) {
		QAction* editContact = contextMenu.addAction("Edit");
		QAction* result = contextMenu.exec(event->globalPos());
		if (result == editContact) {
			eventStream_->send(boost::make_shared<RequestContactEditorUIEvent>(contact->getJID()));
		}
	}
	else if (GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item)) {
		QAction* renameGroup = contextMenu.addAction("Rename");
		QAction* result = contextMenu.exec(event->globalPos());
		if (result == renameGroup) {
			bool ok;
			QString newName = QInputDialog::getText(NULL, "Rename group", "New name for " + P2QSTRING(group->getDisplayName()), QLineEdit::Normal, P2QSTRING(group->getDisplayName()), &ok);
			if (ok) {
				eventStream_->send(boost::make_shared<RenameGroupUIEvent>(group->getDisplayName(), Q2PSTRING(newName)));
			}
		}
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
