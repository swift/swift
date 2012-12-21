/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <cassert>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/bind.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Base/foreach.h>
#include <Swift/Controllers/HighlightManager.h>
#include <Swift/Controllers/Highlighter.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swift/Controllers/SettingConstants.h>

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
	, storingSettings_(false)
{
	loadSettings();
	settings_->onSettingChanged.connect(boost::bind(&HighlightManager::handleSettingChanged, this, _1));
}

void HighlightManager::handleSettingChanged(const std::string& settingPath)
{
	if (!storingSettings_ && SettingConstants::HIGHLIGHT_RULES.getKey() == settingPath) {
		loadSettings();
	}
}

void HighlightManager::loadSettings()
{
	std::string highlightRules = settings_->getSetting(SettingConstants::HIGHLIGHT_RULES);
	if (highlightRules == "@") {
		rules_ = getDefaultRules();
	} else {
		rules_ = rulesFromString(highlightRules);
	}
}

std::string HighlightManager::rulesToString() const
{
	std::string s;
	foreach (HighlightRule r, rules_) {
		s += r.toString() + '\f';
	}
	if (s.size()) {
		s.erase(s.end() - 1);
	}
	return s;
}

std::vector<HighlightRule> HighlightManager::rulesFromString(const std::string& rulesString)
{
	std::vector<HighlightRule> rules;
	std::string s(rulesString);
	typedef boost::split_iterator<std::string::iterator> split_iterator;
	for (split_iterator it = boost::make_split_iterator(s, boost::first_finder("\f")); it != split_iterator(); ++it) {
		HighlightRule r = HighlightRule::fromString(boost::copy_range<std::string>(*it));
		if (!r.isEmpty()) {
			rules.push_back(r);
		}
	}
	return rules;
}

std::vector<HighlightRule> HighlightManager::getDefaultRules()
{
	std::vector<HighlightRule> rules;
	HighlightRule r;
	r.setMatchChat(true);
	r.getAction().setPlaySound(true);
	rules.push_back(r);
	return rules;
}

void HighlightManager::storeSettings()
{
	storingSettings_ = true;	// don't reload settings while saving
	settings_->storeSetting(SettingConstants::HIGHLIGHT_RULES, rulesToString());
	storingSettings_ = false;
}

HighlightRule HighlightManager::getRule(int index) const
{
	assert(index >= 0 && boost::numeric_cast<std::vector<std::string>::size_type>(index) < rules_.size());
	return rules_[index];
}

void HighlightManager::setRule(int index, const HighlightRule& rule)
{
	assert(index >= 0 && boost::numeric_cast<std::vector<std::string>::size_type>(index) < rules_.size());
	rules_[index] = rule;
	storeSettings();
}

void HighlightManager::insertRule(int index, const HighlightRule& rule)
{
	assert(index >= 0 && boost::numeric_cast<std::vector<std::string>::size_type>(index) <= rules_.size());
	rules_.insert(rules_.begin() + index, rule);
	storeSettings();
}

void HighlightManager::removeRule(int index)
{
	assert(index >= 0 && boost::numeric_cast<std::vector<std::string>::size_type>(index) < rules_.size());
	rules_.erase(rules_.begin() + index);
	storeSettings();
}

Highlighter* HighlightManager::createHighlighter()
{
	return new Highlighter(this);
}

}
