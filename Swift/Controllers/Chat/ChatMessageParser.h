/*
 * Copyright (c) 2013-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

namespace Swift {

	class ChatMessageParser {
		public:
			ChatMessageParser(const std::map<std::string, std::string>& emoticons, HighlightRulesListPtr highlightRules, bool mucMode = false);
			ChatWindow::ChatMessage parseMessageBody(const std::string& body, const std::string& nick = "", bool senderIsSelf = false);
		private:
			ChatWindow::ChatMessage emoticonHighlight(const ChatWindow::ChatMessage& parsedMessage);
			ChatWindow::ChatMessage splitHighlight(const ChatWindow::ChatMessage& parsedMessage, const std::string& nick);
			std::map<std::string, std::string> emoticons_;
			HighlightRulesListPtr highlightRules_;
			bool mucMode_;
	};
}
