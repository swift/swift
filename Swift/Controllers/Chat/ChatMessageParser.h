/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swift/Controllers/Highlighting/HighlightConfiguration.h>
#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

namespace Swift {

    /**
     * @brief The ChatMessageParser class takes an emoticon map, a \ref HighlightConfiguration, and a boolean that indicates if the message context is in a MUC or not.
     * The class handles parsing a message string and identifies emoticons, URLs, and various highlights.
     */
    class ChatMessageParser {
        public:
            enum class Mode { Chat, GroupChat };

        public:
            ChatMessageParser(const std::map<std::string, std::string>& emoticons, std::shared_ptr<HighlightConfiguration> highlightConfiguration, Mode mode = Mode::Chat);

            void setNick(const std::string& nick) { nick_ = nick; }
            std::string getNick() const { return nick_; }

            ChatWindow::ChatMessage parseMessageBody(const std::string& body, const std::string& sender = "", bool senderIsSelf = false);

        private:
            ChatWindow::ChatMessage emoticonHighlight(const ChatWindow::ChatMessage& parsedMessage);
            ChatWindow::ChatMessage splitHighlight(const ChatWindow::ChatMessage& parsedMessage);
            ChatWindow::ChatMessage fullMessageHighlight(const ChatWindow::ChatMessage& parsedMessage, const std::string& sender);

        private:
            std::map<std::string, std::string> emoticons_;
            std::shared_ptr<HighlightConfiguration> highlightConfiguration_;
            Mode mode_;
            std::string nick_;
    };
}
