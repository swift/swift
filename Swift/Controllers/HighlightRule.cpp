/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/lambda/lambda.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Base/Regex.h>
#include <Swift/Controllers/HighlightRule.h>

namespace Swift {

HighlightRule::HighlightRule()
	: nickIsKeyword_(false)
	, matchCase_(false)
	, matchWholeWords_(false)
	, matchChat_(false)
	, matchMUC_(false)
{
}

boost::regex HighlightRule::regexFromString(const std::string & s) const
{
	std::string escaped = Regex::escape(s);
	std::string word = matchWholeWords_ ? "\\b" : "";
	boost::regex::flag_type flags = boost::regex::normal;
	if (!matchCase_) {
		flags |= boost::regex::icase;
	}
	return boost::regex(word + escaped + word, flags);
}

void HighlightRule::updateRegex() const
{
	keywordRegex_.clear();
	foreach (const std::string & k, keywords_) {
		keywordRegex_.push_back(regexFromString(k));
	}
	senderRegex_.clear();
	foreach (const std::string & s, senders_) {
		senderRegex_.push_back(regexFromString(s));
	}
}

std::string HighlightRule::boolToString(bool b)
{
	return b ? "1" : "0";
}

bool HighlightRule::boolFromString(const std::string& s)
{
	return s == "1";
}

std::string HighlightRule::toString() const
{
	std::vector<std::string> v;
	v.push_back(boost::join(senders_, "\t"));
	v.push_back(boost::join(keywords_, "\t"));
	v.push_back(boolToString(nickIsKeyword_));
	v.push_back(boolToString(matchChat_));
	v.push_back(boolToString(matchMUC_));
	v.push_back(boolToString(matchCase_));
	v.push_back(boolToString(matchWholeWords_));
	v.push_back(boolToString(action_.highlightText()));
	v.push_back(action_.getTextColor());
	v.push_back(action_.getTextBackground());
	v.push_back(boolToString(action_.playSound()));
	v.push_back(action_.getSoundFile());
	return boost::join(v, "\n");
}

HighlightRule HighlightRule::fromString(const std::string& s)
{
	std::vector<std::string> v;
	boost::split(v, s, boost::is_any_of("\n"));

	HighlightRule r;
	size_t i = 0;
	try {
		boost::split(r.senders_, v.at(i++), boost::is_any_of("\t"));
		r.senders_.erase(std::remove_if(r.senders_.begin(), r.senders_.end(), boost::lambda::_1 == ""), r.senders_.end());
		boost::split(r.keywords_, v.at(i++), boost::is_any_of("\t"));
		r.keywords_.erase(std::remove_if(r.keywords_.begin(), r.keywords_.end(), boost::lambda::_1 == ""), r.keywords_.end());
		r.nickIsKeyword_ = boolFromString(v.at(i++));
		r.matchChat_ = boolFromString(v.at(i++));
		r.matchMUC_ = boolFromString(v.at(i++));
		r.matchCase_ = boolFromString(v.at(i++));
		r.matchWholeWords_ = boolFromString(v.at(i++));
		r.action_.setHighlightText(boolFromString(v.at(i++)));
		r.action_.setTextColor(v.at(i++));
		r.action_.setTextBackground(v.at(i++));
		r.action_.setPlaySound(boolFromString(v.at(i++)));
		r.action_.setSoundFile(v.at(i++));
	}
	catch (std::out_of_range) {
		// this may happen if more properties are added to HighlightRule object, etc...
		// in such case, default values (set by default constructor) will be used
	}

	r.updateRegex();

	return r;
}

bool HighlightRule::isMatch(const std::string& body, const std::string& sender, const std::string& nick, MessageType messageType) const
{
	if ((messageType == HighlightRule::ChatMessage && matchChat_) || (messageType == HighlightRule::MUCMessage && matchMUC_)) {

		bool matchesKeyword = keywords_.empty() && (nick.empty() || !nickIsKeyword_);
		bool matchesSender = senders_.empty();

		foreach (const boost::regex & rx, keywordRegex_) {
			if (boost::regex_search(body, rx)) {
				matchesKeyword = true;
				break;
			}
		}

		if (!matchesKeyword && nickIsKeyword_ && !nick.empty()) {
			if (boost::regex_search(body, regexFromString(nick))) {
				matchesKeyword = true;
			}
		}

		foreach (const boost::regex & rx, senderRegex_) {
			if (boost::regex_search(sender, rx)) {
				matchesSender = true;
				break;
			}
		}

		if (matchesKeyword && matchesSender) {
			return true;
		}
	}

	return false;
}

void HighlightRule::setSenders(const std::vector<std::string>& senders)
{
	senders_ = senders;
	updateRegex();
}

void HighlightRule::setKeywords(const std::vector<std::string>& keywords)
{
	keywords_ = keywords;
	updateRegex();
}

void HighlightRule::setNickIsKeyword(bool nickIsKeyword)
{
	nickIsKeyword_ = nickIsKeyword;
	updateRegex();
}

void HighlightRule::setMatchCase(bool matchCase)
{
	matchCase_ = matchCase;
	updateRegex();
}

void HighlightRule::setMatchWholeWords(bool matchWholeWords)
{
	matchWholeWords_ = matchWholeWords;
	updateRegex();
}

void HighlightRule::setMatchChat(bool matchChat)
{
	matchChat_ = matchChat;
	updateRegex();
}

void HighlightRule::setMatchMUC(bool matchMUC)
{
	matchMUC_ = matchMUC;
	updateRegex();
}

bool HighlightRule::isEmpty() const
{
	return senders_.empty() && keywords_.empty() && !nickIsKeyword_ && !matchChat_ && !matchMUC_ && action_.isEmpty();
}

}
