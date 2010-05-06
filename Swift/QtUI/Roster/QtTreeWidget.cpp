/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Roster/QtTreeWidget.h"

#include "Swiften/Base/Platform.h"
#include "Swiften/Roster/ContactRosterItem.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UIEvents/RequestChatUIEvent.h"

#include <qdebug.h>
#include <QMenu>
#include <QContextMenuEvent>

namespace Swift {

QtTreeWidget::QtTreeWidget(UIEventStream* eventStream, QWidget* parent) : QTreeView(parent) {
	eventStream_ = eventStream;
	model_ = new RosterModel(this);
	setModel(model_);
	delegate_ = new RosterDelegate(this);
	setItemDelegate(delegate_);
	setHeaderHidden(true);
	contextMenu_ = NULL;
#ifdef SWIFT_PLATFORM_MACOSX
	setAlternatingRowColors(true);
#endif
	expandAll();
	setAnimated(true);
	setIndentation(0);
	setRootIsDecorated(true);
	connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(handleItemActivated(const QModelIndex&)));
//	connect(model_, SIGNAL(itemExpanded(const QModelIndex&, bool)), this, SLOT(handleModelItemExpanded(const QModelIndex&, bool)));
//	connect(model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(handleDataChanged(const QModelIndex&, const QModelIndex&)));
//	connect(this, SIGNAL(expanded(const QModelIndex&)), this, SLOT(handleExpanded(const QModelIndex&)));
//	connect(this, SIGNAL(collapsed(const QModelIndex&)), this, SLOT(handleCollapsed(const QModelIndex&)));
}

QtTreeWidget::~QtTreeWidget() {
	delete model_;
	delete delegate_;
}

void QtTreeWidget::setRosterModel(Roster* roster) {
	model_->setRoster(roster);
	expandAll();
}

void QtTreeWidget::setContextMenu(QtContextMenu* contextMenu) {
	contextMenu_ = contextMenu;
}

QtTreeWidgetItem* QtTreeWidget::getRoot() {
	return treeRoot_;
}

void QtTreeWidget::handleItemActivated(const QModelIndex& index) {
	RosterItem* item = static_cast<RosterItem*>(index.internalPointer());
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
	if (contact) {
		eventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(contact->getJID())));
	}
}

void QtTreeWidget::contextMenuEvent(QContextMenuEvent* event) {
	if (!contextMenu_) {
		return;
	}
	QModelIndex index = indexAt(event->pos());
	RosterItem* item = index.isValid() ? static_cast<RosterItem*>(index.internalPointer()) : NULL;
	if (item) {
		contextMenu_->show(item);
	}
}

//void QtTreeWidget::handleExpanded(const QModelIndex& index) {
//	QtTreeWidgetItem* qtItem = static_cast<QtTreeWidgetItem*>(index.internalPointer());
//	if (qtItem) {
//		qtItem->setExpanded(true);
//	}
//}

//void QtTreeWidget::handleCollapsed(const QModelIndex& index) {
//	QtTreeWidgetItem* qtItem = static_cast<QtTreeWidgetItem*>(index.internalPointer());
//	if (qtItem) {
//		qtItem->setExpanded(false);
//	}
//}

//void QtTreeWidget::handleModelItemExpanded(const QModelIndex& index, bool shouldExpand) {
//	if (this->isExpanded(index) == shouldExpand) {
//		return;
//	}
//	//setExpanded(index, shouldExpand);
//	if (shouldExpand) {
//		expand(index);
//		emit expanded(index);
//	} else {
//		collapse(index);
//		emit collapsed(index);
//	}
//}

// void QtTreeWidget::handleDataChanged(const QModelIndex& topLeft, const QModelIndex& /*bottomRight*/) {
// 	//in our model, this is only thrown with topLeft == bottomRight
// 	if (!topLeft.isValid()) {
// 		return;
// 	}
// 	QtTreeWidgetItem* qtItem = static_cast<QtTreeWidgetItem*>(topLeft.internalPointer());
// 	if (qtItem) {
// 		setExpanded(topLeft, qtItem->isExpanded());
// 		//qDebug() << "Item changed, passing expanded state to view: " << qtItem->isExpanded() << " giving an expanded state of " << isExpanded(topLeft);
// 	}

// }

void QtTreeWidget::drawBranches(QPainter*, const QRect&, const QModelIndex&) const {
}

void QtTreeWidget::show() {
	QWidget::show();
}

}
