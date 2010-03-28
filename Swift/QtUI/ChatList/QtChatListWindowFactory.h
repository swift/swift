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
