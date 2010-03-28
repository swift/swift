#pragma once

#include "Swift/Controllers/UIInterfaces/ChatListWindow.h"

namespace Swift {
	class UIEventStream;
	class ChatListWindowFactory {
		public:
			virtual ChatListWindow* createWindow(UIEventStream* uiEventStream) = 0;	
	};
}
