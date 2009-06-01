#include "QtTreeWidget.h"

#include "Swiften/Base/Platform.h"
#include "Swiften/Roster/OpenChatRosterAction.h"

namespace Swift {

QtTreeWidget::QtTreeWidget(QWidget* parent) : QTreeWidget(parent) {
	setHeaderHidden(true);
#ifdef SWIFT_PLATFORM_MACOSX
	setAlternatingRowColors(true);
#endif
	setAnimated(true);
	setIndentation(0);
  setRootIsDecorated(true);
	connect(this, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(handleItemActivated(QTreeWidgetItem*, int)));
}

void QtTreeWidget::handleItemActivated(QTreeWidgetItem* item, int column) {
	QtTreeWidgetItem* qtItem = dynamic_cast<QtTreeWidgetItem*>(item);
	if (qtItem) {
		qtItem->performUserAction(boost::shared_ptr<UserRosterAction>(new OpenChatRosterAction()));
	}
}

void QtTreeWidget::drawBranches(QPainter*, const QRect&, const QModelIndex&) const {
}

}
