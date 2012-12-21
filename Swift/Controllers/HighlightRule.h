/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <vector>
#include <string>

#include <boost/regex.hpp>

#include <Swift/Controllers/HighlightAction.h>

namespace Swift {

	class HighlightRule {
		public:
			HighlightRule();

			enum MessageType { ChatMessage, MUCMessage };

			bool isMatch(const std::string& body, const std::string& sender, const std::string& nick, MessageType) const;

			const HighlightAction& getAction() const { return action_; }
			HighlightAction& getAction() { return action_; }

			static HighlightRule fromString(const std::string&);
			std::string toString() const;

			const std::vector<std::string>& getSenders() const { return senders_; }
			void setSenders(const std::vector<std::string>&);

			const std::vector<std::string>& getKeywords() const { return keywords_; }
			void setKeywords(const std::vector<std::string>&);

			bool getNickIsKeyword() const { return nickIsKeyword_; }
			void setNickIsKeyword(bool);

			bool getMatchCase() const { return matchCase_; }
			void setMatchCase(bool);

			bool getMatchWholeWords() const { return matchWholeWords_; }
			void setMatchWholeWords(bool);

			bool getMatchChat() const { return matchChat_; }
			void setMatchChat(bool);

			bool getMatchMUC() const { return matchMUC_; }
			void setMatchMUC(bool);

			bool isEmpty() const;

		private:
			static std::string boolToString(bool);
			static bool boolFromString(const std::string&);

			std::vector<std::string> senders_;
			std::vector<std::string> keywords_;
			bool nickIsKeyword_;

			mutable std::vector<boost::regex> senderRegex_;
			mutable std::vector<boost::regex> keywordRegex_;
			void updateRegex() const;
			boost::regex regexFromString(const std::string&) const;

			bool matchCase_;
			bool matchWholeWords_;

			bool matchChat_;
			bool matchMUC_;

			HighlightAction action_;
	};

}
