/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
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
			HighlightManager(SettingsProvider* settings);

			Highlighter* createHighlighter();

			const std::vector<HighlightRule>& getRules() const { return rules_; }
			HighlightRule getRule(int index) const;
			void setRule(int index, const HighlightRule& rule);
			void insertRule(int index, const HighlightRule& rule);
			void removeRule(int index);

			boost::signal<void (const HighlightAction&)> onHighlight;

		private:
			void handleSettingChanged(const std::string& settingPath);

			std::string rulesToString() const;
			static std::vector<HighlightRule> rulesFromString(const std::string&);
			static std::vector<HighlightRule> getDefaultRules();

			SettingsProvider* settings_;
			bool storingSettings_;
			void storeSettings();
			void loadSettings();

			std::vector<HighlightRule> rules_;
	};

}
