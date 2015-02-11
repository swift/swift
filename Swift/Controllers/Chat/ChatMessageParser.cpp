/*
 * Copyright (c) 2013-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Chat/ChatMessageParser.h>

#include <vector>
#include <utility>

#include <boost/smart_ptr/make_shared.hpp>
#include <boost/algorithm/string.hpp>

#include <Swiften/Base/Regex.h>
#include <Swiften/Base/foreach.h>

#include <SwifTools/Linkify.h>


namespace Swift {

	ChatMessageParser::ChatMessageParser(const std::map<std::string, std::string>& emoticons, HighlightRulesListPtr highlightRules, bool mucMode)
	: emoticons_(emoticons), highlightRules_(highlightRules), mucMode_(mucMode) {
	}

	typedef std::pair<std::string, std::string> StringPair;

	ChatWindow::ChatMessage ChatMessageParser::parseMessageBody(const std::string& body, const std::string& nick, bool senderIsSelf) {
		ChatWindow::ChatMessage parsedMessage;
		std::string remaining = body;
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
						parsedMessage.append(boost::make_shared<ChatWindow::ChatURIMessagePart>(part));
					}
					else {
						parsedMessage.append(boost::make_shared<ChatWindow::ChatTextMessagePart>(part));
					}
				}
			}
		}

		/* do emoticon substitution */
		parsedMessage = emoticonHighlight(parsedMessage);

		if (!senderIsSelf) { /* do not highlight our own messsages */
			/* do word-based color highlighting */
			parsedMessage = splitHighlight(parsedMessage, nick);
		}

		return parsedMessage;
	}

	ChatWindow::ChatMessage ChatMessageParser::emoticonHighlight(const ChatWindow::ChatMessage& message)
	{
		ChatWindow::ChatMessage parsedMessage = message;

		std::string regexString;
		/* Parse two, emoticons */
		foreach (StringPair emoticon, emoticons_) {
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
			foreach (boost::shared_ptr<ChatWindow::ChatMessagePart> part, parsedMessage.getParts()) {
				boost::shared_ptr<ChatWindow::ChatTextMessagePart> textPart;
				if ((textPart = boost::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(part))) {
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
								newMessage.append(boost::make_shared<ChatWindow::ChatTextMessagePart>(std::string(start, matchStart)));
							}
							boost::shared_ptr<ChatWindow::ChatEmoticonMessagePart> emoticonPart = boost::make_shared<ChatWindow::ChatEmoticonMessagePart>();
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
							newMessage.append(boost::make_shared<ChatWindow::ChatTextMessagePart>(std::string(start, text.end())));
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
			parsedMessage = newMessage;

		}
		return parsedMessage;
	}

	ChatWindow::ChatMessage ChatMessageParser::splitHighlight(const ChatWindow::ChatMessage& message, const std::string& nick)
	{
		ChatWindow::ChatMessage parsedMessage = message;

		for (size_t i = 0; i < highlightRules_->getSize(); ++i) {
			const HighlightRule& rule = highlightRules_->getRule(i);
			if (rule.getMatchMUC() && !mucMode_) {
				continue; /* this rule only applies to MUC's, and this is a CHAT */
			} else if (rule.getMatchChat() && mucMode_) {
				continue; /* this rule only applies to CHAT's, and this is a MUC */
			} else if (rule.getAction().getTextBackground().empty() && rule.getAction().getTextColor().empty()) {
				continue; /* do not try to highlight text, if no highlight color is specified */
			}
			const std::vector<boost::regex> keywordRegex = rule.getKeywordRegex(nick);
			foreach(const boost::regex& regex, keywordRegex) {
				ChatWindow::ChatMessage newMessage;
				foreach (boost::shared_ptr<ChatWindow::ChatMessagePart> part, parsedMessage.getParts()) {
					boost::shared_ptr<ChatWindow::ChatTextMessagePart> textPart;
					if ((textPart = boost::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(part))) {
						try {
							boost::match_results<std::string::const_iterator> match;
							const std::string& text = textPart->text;
							std::string::const_iterator start = text.begin();
							while (regex_search(start, text.end(), match, regex)) {
								std::string::const_iterator matchStart = match[0].first;
								std::string::const_iterator matchEnd = match[0].second;
								if (start != matchStart) {
									/* If we're skipping over plain text since the previous emoticon, record it as plain text */
									newMessage.append(boost::make_shared<ChatWindow::ChatTextMessagePart>(std::string(start, matchStart)));
								}
								boost::shared_ptr<ChatWindow::ChatHighlightingMessagePart> highlightPart = boost::make_shared<ChatWindow::ChatHighlightingMessagePart>();
								highlightPart->text = match.str();
								highlightPart->foregroundColor = rule.getAction().getTextColor();
								highlightPart->backgroundColor = rule.getAction().getTextBackground();
								newMessage.append(highlightPart);
								start = matchEnd;
							}
							if (start != text.end()) {
								/* If there's plain text after the last emoticon, record it */
								newMessage.append(boost::make_shared<ChatWindow::ChatTextMessagePart>(std::string(start, text.end())));
							}
						}
						catch (std::runtime_error) {
							/* Basically too expensive to compute the regex results and it gave up, so pass through as text */
							newMessage.append(part);
						}
					} else {
						newMessage.append(part);
					}
				}
				parsedMessage = newMessage;
			}
		}

		return parsedMessage;
	}
}
