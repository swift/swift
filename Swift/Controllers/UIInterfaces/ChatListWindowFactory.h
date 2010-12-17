/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIInterfaces/ChatListWindow.h"

namespace Swift {
	class UIEventStream;
	class ChatListWindowFactory {
		public:
			virtual ~ChatListWindowFactory() {}
			virtual ChatListWindow* createChatListWindow(UIEventStream* uiEventStream) = 0;
	};
}
