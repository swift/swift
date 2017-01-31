/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Chat/ChatMessageParser.h>

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

#include <Swiften/Base/Regex.h>
#include <Swiften/Base/String.h>

#include <SwifTools/Linkify.h>

namespace Swift {

    ChatMessageParser::ChatMessageParser(const std::map<std::string, std::string>& emoticons, std::shared_ptr<HighlightConfiguration> highlightConfiguration, Mode mode) : emoticons_(emoticons), highlightConfiguration_(highlightConfiguration), mode_(mode) {
    }

    typedef std::pair<std::string, std::string> StringPair;

    ChatWindow::ChatMessage ChatMessageParser::parseMessageBody(const std::string& body, const std::string& senderNickname, bool senderIsSelf) {
        ChatWindow::ChatMessage parsedMessage;

        std::string remaining = body;
        if (boost::starts_with(body, "/me ")) {
           remaining = String::getSplittedAtFirst(body, ' ').second;
           parsedMessage.setIsMeCommand(true);
        }

        /* Parse one, URLs */
        while (!remaining.empty()) {
            bool found = false;
            std::pair<std::vector<std::string>, size_t> links = Linkify::splitLink(remaining);
            remaining = "";
            for (size_t i = 0; i < links.first.size(); i++) {
                const std::string& part = links.first[i];
                if (found) {
                    // Must be on the last part, then
                    remaining = part;
                }
                else {
                    if (i == links.second) {
                        found = true;
                        parsedMessage.append(std::make_shared<ChatWindow::ChatURIMessagePart>(part));
                    }
                    else {
                        parsedMessage.append(std::make_shared<ChatWindow::ChatTextMessagePart>(part));
                    }
                }
            }
        }

        /* do emoticon substitution */
        parsedMessage = emoticonHighlight(parsedMessage);

        if (!senderIsSelf) { /* do not highlight our own messsages */
            // Highlight keywords and own mentions.
            parsedMessage = splitHighlight(parsedMessage);

            // Highlight full message events like, specific sender, general
            // incoming group message, or general incoming direct message.
            parsedMessage = fullMessageHighlight(parsedMessage, senderNickname);
        }

        return parsedMessage;
    }

    ChatWindow::ChatMessage ChatMessageParser::emoticonHighlight(const ChatWindow::ChatMessage& message) {
        ChatWindow::ChatMessage parsedMessage = message;

        std::string regexString;
        /* Parse two, emoticons */
        for (StringPair emoticon : emoticons_) {
            /* Construct a regexp that finds an instance of any of the emoticons inside a group
             * at the start or end of the line, or beside whitespace.
             */
            regexString += regexString.empty() ? "" : "|";
            std::string escaped = "(" + Regex::escape(emoticon.first) + ")";
            regexString += "^" + escaped + "|";
            regexString += escaped + "$|";
            regexString += "\\s" + escaped + "|";
            regexString += escaped + "\\s";

        }
        if (!regexString.empty()) {
            regexString += "";
            boost::regex emoticonRegex(regexString);

            ChatWindow::ChatMessage newMessage;
            for (const auto& part : parsedMessage.getParts()) {
                std::shared_ptr<ChatWindow::ChatTextMessagePart> textPart;
                if ((textPart = std::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(part))) {
                    try {
                        boost::match_results<std::string::const_iterator> match;
                        const std::string& text = textPart->text;
                        std::string::const_iterator start = text.begin();
                        while (regex_search(start, text.end(), match, emoticonRegex)) {
                            int matchIndex = 0;
                            for (matchIndex = 1; matchIndex < static_cast<int>(match.size()); matchIndex++) {
                                if (match[matchIndex].length() > 0) {
                                    //This is the matching subgroup
                                    break;
                                }
                            }
                            std::string::const_iterator matchStart = match[matchIndex].first;
                            std::string::const_iterator matchEnd = match[matchIndex].second;
                            if (start != matchStart) {
                                /* If we're skipping over plain text since the previous emoticon, record it as plain text */
                                newMessage.append(std::make_shared<ChatWindow::ChatTextMessagePart>(std::string(start, matchStart)));
                            }
                            std::shared_ptr<ChatWindow::ChatEmoticonMessagePart> emoticonPart = std::make_shared<ChatWindow::ChatEmoticonMessagePart>();
                            std::string matchString = match[matchIndex].str();
                            std::map<std::string, std::string>::const_iterator emoticonIterator = emoticons_.find(matchString);
                            assert (emoticonIterator != emoticons_.end());
                            const StringPair& emoticon = *emoticonIterator;
                            emoticonPart->imagePath = emoticon.second;
                            emoticonPart->alternativeText = emoticon.first;
                            newMessage.append(emoticonPart);
                            start = matchEnd;
                        }
                        if (start != text.end()) {
                            /* If there's plain text after the last emoticon, record it */
                            newMessage.append(std::make_shared<ChatWindow::ChatTextMessagePart>(std::string(start, text.end())));
                        }

                    }
                    catch (std::runtime_error) {
                        /* Basically too expensive to compute the regex results and it gave up, so pass through as text */
                        newMessage.append(part);
                    }
                }
                else {
                    newMessage.append(part);
                }
            }
            parsedMessage.setParts(newMessage.getParts());
        }

        return parsedMessage;
    }

    ChatWindow::ChatMessage ChatMessageParser::splitHighlight(const ChatWindow::ChatMessage& message) {
        auto keywordToRegEx = [](const std::string& keyword, bool matchCaseSensitive) {
            std::string escaped = Regex::escape(keyword);
            boost::regex::flag_type flags = boost::regex::normal;
            if (!matchCaseSensitive) {
                flags |= boost::regex::icase;
            }
            return boost::regex("\\b" + escaped + "\\b", flags);
        };

        auto highlightKeywordInChatMessage = [&](const ChatWindow::ChatMessage& message, const std::string& keyword, bool matchCaseSensitive, const HighlightAction& action) {
            ChatWindow::ChatMessage resultMessage;

            for (const auto& part : message.getParts()) {
                std::shared_ptr<ChatWindow::ChatTextMessagePart> textPart;
                if ((textPart = std::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(part))) {
                    try {
                        boost::match_results<std::string::const_iterator> match;
                        const std::string& text = textPart->text;
                        std::string::const_iterator start = text.begin();
                        while (regex_search(start, text.end(), match, keywordToRegEx(keyword, matchCaseSensitive))) {
                            std::string::const_iterator matchStart = match[0].first;
                            std::string::const_iterator matchEnd = match[0].second;
                            if (start != matchStart) {
                                /* If we're skipping over plain text since the previous emoticon, record it as plain text */
                                resultMessage.append(std::make_shared<ChatWindow::ChatTextMessagePart>(std::string(start, matchStart)));
                            }
                            std::shared_ptr<ChatWindow::ChatHighlightingMessagePart> highlightPart = std::make_shared<ChatWindow::ChatHighlightingMessagePart>();
                            highlightPart->text = match.str();
                            highlightPart->action = action;
                            resultMessage.append(highlightPart);
                            start = matchEnd;
                        }
                        if (start != text.end()) {
                            /* If there's plain text after the last emoticon, record it */
                            resultMessage.append(std::make_shared<ChatWindow::ChatTextMessagePart>(std::string(start, text.end())));
                        }
                    }
                    catch (std::runtime_error) {
                        /* Basically too expensive to compute the regex results and it gave up, so pass through as text */
                        resultMessage.append(part);
                    }
                } else {
                    resultMessage.append(part);
                }
            }
            return resultMessage;
        };

        ChatWindow::ChatMessage parsedMessage = message;

        // detect mentions of own nickname
        HighlightAction ownMentionKeywordAction = highlightConfiguration_->ownMentionAction;
        ownMentionKeywordAction.setSoundFilePath(boost::optional<std::string>());
        ownMentionKeywordAction.setSystemNotificationEnabled(false);
        if (!getNick().empty() && !highlightConfiguration_->ownMentionAction.isEmpty()) {
            auto nicknameHighlightedMessage = highlightKeywordInChatMessage(parsedMessage, nick_, false, ownMentionKeywordAction);
            auto highlightedParts = nicknameHighlightedMessage.getParts();
            auto ownNicknamePart = std::find_if(highlightedParts.begin(), highlightedParts.end(), [&](std::shared_ptr<ChatWindow::ChatMessagePart>& part){
                auto highlightPart = std::dynamic_pointer_cast<ChatWindow::ChatHighlightingMessagePart>(part);
                if (highlightPart && highlightPart->text == nick_) {
                    return true;
                }
                return false;
            });
            if (ownNicknamePart != highlightedParts.end()) {
                parsedMessage.setHighlightActionOwnMention(highlightConfiguration_->ownMentionAction);
            }
            parsedMessage.setParts(nicknameHighlightedMessage.getParts());
        }

        // detect keywords
        for (const auto& keywordHighlight : highlightConfiguration_->keywordHighlights) {
            if (keywordHighlight.keyword.empty() || keywordHighlight.action.isEmpty()) {
                continue;
            }
            auto newMessage = highlightKeywordInChatMessage(parsedMessage, keywordHighlight.keyword, keywordHighlight.matchCaseSensitive, keywordHighlight.action);
            parsedMessage.setParts(newMessage.getParts());
        }

        return parsedMessage;
    }

    ChatWindow::ChatMessage ChatMessageParser::fullMessageHighlight(const ChatWindow::ChatMessage& parsedMessage, const std::string& sender) {
        auto fullHighlightedMessage = parsedMessage;

        // contact highlighting
        for (const auto& contactHighlight : highlightConfiguration_->contactHighlights) {
            if (sender == contactHighlight.name) {
                fullHighlightedMessage.setHighlightActionSender(contactHighlight.action);
                break;
            }
        }

        // general incoming messages
        HighlightAction groupAction;
        HighlightAction chatAction;

        switch (mode_) {
            case Mode::GroupChat:
                groupAction.setSoundFilePath(highlightConfiguration_->playSoundOnIncomingGroupchatMessages ? boost::optional<std::string>("") : boost::optional<std::string>());
                groupAction.setSystemNotificationEnabled(highlightConfiguration_->showNotificationOnIncomingGroupchatMessages);
                fullHighlightedMessage.setHighlightActionGroupMessage(groupAction);
                break;

            case Mode::Chat:
                chatAction.setSoundFilePath(highlightConfiguration_->playSoundOnIncomingDirectMessages ? boost::optional<std::string>("") : boost::optional<std::string>());
                chatAction.setSystemNotificationEnabled(highlightConfiguration_->showNotificationOnIncomingDirectMessages);
                fullHighlightedMessage.setHighlightActonDirectMessage(chatAction);
                break;
        }

        return fullHighlightedMessage;
    }
}
