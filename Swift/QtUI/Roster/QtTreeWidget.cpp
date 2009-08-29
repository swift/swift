#include "Roster/QtTreeWidget.h"

#include "Swiften/Base/Platform.h"
#include "Swiften/Roster/OpenChatRosterAction.h"

#include <qdebug.h>

namespace Swift {

QtTreeWidget::QtTreeWidget(QWidget* parent) : QTreeView(parent) {
	treeRoot_ = new QtTreeWidgetItem(NULL);
	model_ = new RosterModel();
	model_->setRoot(treeRoot_);
	setModel(model_);
	delegate_ = new RosterDelegate();
	setItemDelegate(delegate_);
	setHeaderHidden(true);
#ifdef SWIFT_PLATFORM_MACOSX
	setAlternatingRowColors(true);
#endif
	setAnimated(true);
	setIndentation(0);
	setRootIsDecorated(true);
	connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(handleItemActivated(const QModelIndex&)));
	connect(model_, SIGNAL(itemExpanded(const QModelIndex&, bool)), this, SLOT(handleModelItemExpanded(const QModelIndex&, bool)));
	connect(model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(handleDataChanged(const QModelIndex&, const QModelIndex&)));
	connect(this, SIGNAL(expanded(const QModelIndex&)), this, SLOT(handleExpanded(const QModelIndex&)));
	connect(this, SIGNAL(collapsed(const QModelIndex&)), this, SLOT(handleCollapsed(const QModelIndex&)));
}

QtTreeWidget::~QtTreeWidget() {
	delete model_;
	delete delegate_;
}

QtTreeWidgetItem* QtTreeWidget::getRoot() {
	return treeRoot_;
}

void QtTreeWidget::handleItemActivated(const QModelIndex& index) {
	QtTreeWidgetItem* qtItem = static_cast<QtTreeWidgetItem*>(index.internalPointer());
	if (qtItem) {
		qtItem->performUserAction(boost::shared_ptr<UserRosterAction>(new OpenChatRosterAction()));
	}
}

void QtTreeWidget::handleExpanded(const QModelIndex& index) {
	QtTreeWidgetItem* qtItem = static_cast<QtTreeWidgetItem*>(index.internalPointer());
	if (qtItem) {
		qtItem->setExpanded(true);
	}
}

void QtTreeWidget::handleCollapsed(const QModelIndex& index) {
	QtTreeWidgetItem* qtItem = static_cast<QtTreeWidgetItem*>(index.internalPointer());
	if (qtItem) {
		qtItem->setExpanded(false);
	}
}

void QtTreeWidget::handleModelItemExpanded(const QModelIndex& index, bool shouldExpand) {
	if (this->isExpanded(index) == shouldExpand) {
		return;
	}
	//setExpanded(index, shouldExpand);
	if (shouldExpand) {
		expand(index);
		emit expanded(index);
	} else {
		collapse(index);
		emit collapsed(index);
	}
}

void QtTreeWidget::handleDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {
	Q_UNUSED(bottomRight);
	//in our model, this is only thrown with topLeft == bottomRight
	if (!topLeft.isValid()) {
		return;
	}
	QtTreeWidgetItem* qtItem = static_cast<QtTreeWidgetItem*>(topLeft.internalPointer());
	if (qtItem) {
		//qDebug() << "Item changed, passing expanded state to view: " << qtItem->isExpanded();
		setExpanded(topLeft, qtItem->isExpanded());
	}

}

void QtTreeWidget::drawBranches(QPainter*, const QRect&, const QModelIndex&) const {
}

void QtTreeWidget::show() {
	QWidget::show();
}

}
