/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

namespace Swift {

	class ChatMessageParser {
		public:
			ChatMessageParser(const std::map<std::string, std::string>& emoticons, HighlightRulesListPtr highlightRules, bool mucMode = false);
			ChatWindow::ChatMessage parseMessageBody(const std::string& body, bool senderIsSelf = false);
		private:
			ChatWindow::ChatMessage emoticonHighlight(const ChatWindow::ChatMessage& parsedMessage);
			ChatWindow::ChatMessage splitHighlight(const ChatWindow::ChatMessage& parsedMessage);
			std::map<std::string, std::string> emoticons_;
			HighlightRulesListPtr highlightRules_;
			bool mucMode_;
	};
}
