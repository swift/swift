/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <string>

#include <boost/regex.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <Swift/Controllers/HighlightAction.h>

namespace Swift {

	class HighlightRule {
		public:
			HighlightRule();

			enum MessageType { ChatMessage, MUCMessage };

			bool isMatch(const std::string& body, const std::string& sender, const std::string& nick, MessageType) const;

			const HighlightAction& getAction() const { return action_; }
			HighlightAction& getAction() { return action_; }

			const std::vector<std::string>& getSenders() const { return senders_; }
			void setSenders(const std::vector<std::string>&);
			const std::vector<boost::regex>& getSenderRegex() const { return senderRegex_; }

			const std::vector<std::string>& getKeywords() const { return keywords_; }
			void setKeywords(const std::vector<std::string>&);
			std::vector<boost::regex> getKeywordRegex(const std::string& nick) const;

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
			friend class boost::serialization::access;
			template<class Archive> void serialize(Archive & ar, const unsigned int version);

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

	template<class Archive>
	void HighlightRule::serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar & senders_;
		ar & keywords_;
		ar & nickIsKeyword_;
		ar & matchChat_;
		ar & matchMUC_;
		ar & matchCase_;
		ar & matchWholeWords_;
		ar & action_;
		updateRegex();
	}

}
