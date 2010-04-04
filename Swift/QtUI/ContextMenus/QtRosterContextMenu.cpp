#include "Swift/QtUI/ContextMenus/QtRosterContextMenu.h"

#include <QMenu>
#include <QDebug>

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swift/Controllers/UIEvents/RemoveItemRosterAction.h"
#include "Swift/QtUI/Roster/QtTreeWidgetItem.h"
#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {

QtRosterContextMenu::QtRosterContextMenu(UIEventStream* eventStream) {
	eventStream_ = eventStream;
}

void QtRosterContextMenu::show(QtTreeWidgetItem* item) {
	if (!item->isContact()) {
		return;
	}
	item_ = item;
	QMenu* contextMenu = new QMenu();
	contextMenu->addAction("Remove", this, SLOT(handleRemove()));
	contextMenu->exec(QCursor::pos());
}

void QtRosterContextMenu::handleRemove() {
	item_->performUserAction(boost::shared_ptr<UserRosterAction>(new RemoveItemRosterAction()));
}

}
