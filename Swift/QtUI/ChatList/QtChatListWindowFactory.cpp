/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/ChatList/QtChatListWindowFactory.h"

#include "Swift/QtUI/QtMainWindowFactory.h"
#include "Swift/QtUI/QtMainWindow.h"

namespace Swift {

QtChatListWindowFactory::QtChatListWindowFactory(QtMainWindowFactory* mainWindowFactory) {
	mainWindowFactory_ = mainWindowFactory;
}

ChatListWindow* QtChatListWindowFactory::createWindow(UIEventStream* /*uiEventStream*/) {
	return static_cast<QtMainWindow*>(mainWindowFactory_->getLastCreatedWindow())->getChatListWindow();
}

}
