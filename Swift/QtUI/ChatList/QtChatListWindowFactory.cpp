#include "Swift/QtUI/ChatList/QtChatListWindowFactory.h"

#include "Swift/QtUI/QtMainWindowFactory.h"
#include "Swift/QtUI/QtMainWindow.h"

namespace Swift {

QtChatListWindowFactory::QtChatListWindowFactory(QtMainWindowFactory* mainWindowFactory) {
	mainWindowFactory_ = mainWindowFactory;
}

ChatListWindow* QtChatListWindowFactory::createWindow(UIEventStream* /*uiEventStream*/) {
	return ((QtMainWindow*)mainWindowFactory_->getLastCreatedWindow())->getChatListWindow();
}

}
