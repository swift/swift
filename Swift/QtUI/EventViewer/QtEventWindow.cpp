#include "Swift/QtUI/EventViewer/QtEventWindow.h"

#include "Swiften/Base/Platform.h"

namespace Swift {

QtEventWindow::QtEventWindow(QWidget* parent) : QTreeView(parent) {
	model_ = new EventModel();
	setModel(model_);
	delegate_ = new EventDelegate();
	setItemDelegate(delegate_);
	setHeaderHidden(true);
#ifdef SWIFT_PLATFORM_MACOSX
	setAlternatingRowColors(true);
#endif
	setAnimated(true);
	setIndentation(0);
	setRootIsDecorated(true);
}

QtEventWindow::~QtEventWindow() {
	delete model_;
	delete delegate_;
}

void QtEventWindow::addEvent(boost::shared_ptr<Event> event, bool active) {
	model_->addEvent(event, active);
}

void QtEventWindow::removeEvent(boost::shared_ptr<Event> event) {
	model_->removeEvent(event);
}

}
