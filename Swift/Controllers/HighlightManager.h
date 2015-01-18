/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>
#include <string>

#include <Swiften/Base/boost_bsignals.h>
#include <Swift/Controllers/HighlightRule.h>

namespace Swift {

	class SettingsProvider;
	class Highlighter;

	class HighlightManager {
		public:

			class HighlightRulesList {
			public:
				friend class HighlightManager;
				size_t getSize() const { return list_.size(); }
				const HighlightRule& getRule(const size_t index) const { return list_[index]; }
				void addRule(const HighlightRule& rule) { list_.push_back(rule); }
				void combineRules(const HighlightRulesList& rhs) {
					list_.insert(list_.end(), rhs.list_.begin(), rhs.list_.end());
				}
				void setRule(const size_t index, const HighlightRule& rule) {
					list_[index] = rule;
				}
			private:
				std::vector<HighlightRule> list_;
			};

			HighlightManager(SettingsProvider* settings);

			Highlighter* createHighlighter();

			boost::shared_ptr<const HighlightManager::HighlightRulesList> getRules() const { return rules_; }

			bool isDefaultRulesList() const;
			void resetToDefaultRulesList();

			HighlightRule getRule(int index) const;
			void setRule(int index, const HighlightRule& rule);
			void insertRule(int index, const HighlightRule& rule);
			void removeRule(int index);
			void swapRules(const size_t first, const size_t second);
			void storeSettings();
			void loadSettings();

			boost::signal<void (const HighlightAction&)> onHighlight;

		private:
			void handleSettingChanged(const std::string& settingPath);

			std::string rulesToString() const;
			static std::vector<HighlightRule> getDefaultRules();

			SettingsProvider* settings_;
			bool storingSettings_;

			boost::shared_ptr<HighlightManager::HighlightRulesList> rules_;
	};

	typedef boost::shared_ptr<const HighlightManager::HighlightRulesList> HighlightRulesListPtr;

}
