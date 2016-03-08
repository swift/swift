/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/HighlightManager.h>

#include <cassert>

#include <boost/algorithm/string.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/bind.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/regex.hpp>
#include <boost/serialization/vector.hpp>

#include <Swiften/Base/foreach.h>

#include <Swift/Controllers/Highlighter.h>
#include <Swift/Controllers/SettingConstants.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>

/* How does highlighting work?
 *
 * HighlightManager manages a list of if-then rules used to highlight messages.
 * Rule is represented by HighlightRule. Action ("then" part) is HighlightAction.
 *
 *
 * HighlightManager is also used as a factory for Highlighter objects.
 * Each ChatControllerBase has its own Highlighter.
 * Highligher may be customized by using setNick(), etc.
 *
 * ChatControllerBase passes incoming messages to Highlighter and gets HighlightAction in return
 * (first matching rule is returned).
 * If needed, HighlightAction is then passed back to Highlighter for further handling.
 * This results in HighlightManager emiting onHighlight event,
 * which is handled by SoundController to play sound notification
 */

namespace Swift {

HighlightManager::HighlightManager(SettingsProvider* settings)
	: settings_(settings)
	, storingSettings_(false) {
	rules_ = boost::make_shared<HighlightRulesList>();
	loadSettings();
	handleSettingChangedConnection_ = settings_->onSettingChanged.connect(boost::bind(&HighlightManager::handleSettingChanged, this, _1));
}

void HighlightManager::handleSettingChanged(const std::string& settingPath) {
	if (!storingSettings_ && SettingConstants::HIGHLIGHT_RULES.getKey() == settingPath) {
		loadSettings();
	}
}

std::string HighlightManager::rulesToString() const {
	std::stringstream stream;
	boost::archive::text_oarchive archive(stream);
	archive << rules_->list_;
	return stream.str();
}

std::vector<HighlightRule> HighlightManager::getDefaultRules() {
	std::vector<HighlightRule> rules;

	HighlightRule chatNotificationRule;
	chatNotificationRule.setMatchChat(true);
	chatNotificationRule.getAction().setPlaySound(true);
	chatNotificationRule.setMatchWholeWords(true);
	rules.push_back(chatNotificationRule);

	HighlightRule selfMentionMUCRule;
	selfMentionMUCRule.setMatchMUC(true);
	selfMentionMUCRule.getAction().setPlaySound(true);
	selfMentionMUCRule.setNickIsKeyword(true);
	selfMentionMUCRule.setMatchCase(true);
	selfMentionMUCRule.setMatchWholeWords(true);
	rules.push_back(selfMentionMUCRule);

	return rules;
}

HighlightRule HighlightManager::getRule(int index) const {
	assert(index >= 0 && static_cast<size_t>(index) < rules_->getSize());
	return rules_->getRule(static_cast<size_t>(index));
}

void HighlightManager::setRule(int index, const HighlightRule& rule) {
	assert(index >= 0 && static_cast<size_t>(index) < rules_->getSize());
	rules_->list_[static_cast<size_t>(index)] = rule;
}

void HighlightManager::insertRule(int index, const HighlightRule& rule) {
	assert(index >= 0 && boost::numeric_cast<std::vector<std::string>::size_type>(index) <= rules_->getSize());
	rules_->list_.insert(rules_->list_.begin() + index, rule);
}

void HighlightManager::removeRule(int index) {
	assert(index >= 0 && boost::numeric_cast<std::vector<std::string>::size_type>(index) < rules_->getSize());
	rules_->list_.erase(rules_->list_.begin() + index);
}

void HighlightManager::swapRules(const size_t first, const size_t second) {
	assert(first < rules_->getSize());
	assert(second < rules_->getSize());
	const HighlightRule swap = rules_->getRule(first);
	rules_->setRule(first, rules_->getRule(second));
	rules_->setRule(second, swap);
}

void HighlightManager::storeSettings() {
	storingSettings_ = true;	// don't reload settings while saving
	settings_->storeSetting(SettingConstants::HIGHLIGHT_RULES, rulesToString());
	storingSettings_ = false;
}

void HighlightManager::loadSettings() {
	std::string rulesString = settings_->getSetting(SettingConstants::HIGHLIGHT_RULES);
	std::stringstream stream;
	stream << rulesString;
	try {
		boost::archive::text_iarchive archive(stream);
		archive >> rules_->list_;
	} catch (boost::archive::archive_exception&) {
		rules_->list_ = getDefaultRules();
	}
}

Highlighter* HighlightManager::createHighlighter() {
	return new Highlighter(this);
}

bool HighlightManager::isDefaultRulesList() const {
	return getDefaultRules() == rules_->list_;
}

void HighlightManager::resetToDefaultRulesList() {
	rules_->list_ = getDefaultRules();
}

}
