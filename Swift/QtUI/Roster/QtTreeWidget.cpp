#include "Roster/QtTreeWidget.h"

#include "Swiften/Base/Platform.h"
#include "Swiften/Roster/OpenChatRosterAction.h"

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
	connect(model_, SIGNAL(itemExpanded(const QModelIndex&, bool)), this, SLOT(handleItemExpanded(const QModelIndex&, bool)));
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

void QtTreeWidget::handleItemExpanded(const QModelIndex& index, bool expanded) {
	setExpanded(index, expanded);
}

void QtTreeWidget::drawBranches(QPainter*, const QRect&, const QModelIndex&) const {
}

void QtTreeWidget::show() {
	QWidget::show();
}

}
