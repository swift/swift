/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIInterfaces/ChatListWindowFactory.h"

namespace Swift {
	class QtMainWindowFactory;
	class QtChatListWindowFactory : public ChatListWindowFactory{
		public:
			QtChatListWindowFactory(QtMainWindowFactory* mainWindowFactory);
			ChatListWindow* createWindow(UIEventStream* uiEventStream);
		private:
			QtMainWindowFactory* mainWindowFactory_;
	};
}
