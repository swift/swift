/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

namespace Swift {

	class ChatMessageParser {
		public:
			ChatMessageParser(const std::map<std::string, std::string>& emoticons);
			ChatWindow::ChatMessage parseMessageBody(const std::string& body);
		private:
			std::map<std::string, std::string> emoticons_;

	};
}
