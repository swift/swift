#include "Swift/QtUI/EventViewer/QtEventWindowFactory.h"

#include "Swift/QtUI/QtMainWindowFactory.h"
#include "Swift/QtUI/QtMainWindow.h"

namespace Swift {

QtEventWindowFactory::QtEventWindowFactory(QtMainWindowFactory* mainWindowFactory) {
	mainWindowFactory_ = mainWindowFactory;
}

EventWindow* QtEventWindowFactory::createEventWindow() {
	return ((QtMainWindow*)mainWindowFactory_->getLastCreatedWindow())->getEventWindow();
}

}
