/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/signals2.hpp>

#include <Swift/Controllers/Highlighting/HighlightConfiguration.h>

namespace Swift {

    class NickResolver;
    class SettingsProvider;
    class Highlighter;

    class HighlightManager {
        public:
            HighlightManager(SettingsProvider* settings);

            Highlighter* createHighlighter(NickResolver* nickResolver);

            std::shared_ptr<HighlightConfiguration> getConfiguration() const {
                return highlightConfiguration_;
            }

            void setConfiguration(const HighlightConfiguration& config) {
                *highlightConfiguration_ = config;
                storeSettings();
            }

            void resetToDefaultConfiguration();

            void storeSettings();
            void loadSettings();

            boost::signals2::signal<void (const HighlightAction&)> onHighlight;

        private:
            void handleSettingChanged(const std::string& settingPath);

            static HighlightConfiguration getDefaultConfig();

            static HighlightConfiguration highlightConfigurationFromString(const std::string& dataString);
            static std::string highlightConfigurationToString(const HighlightConfiguration& configuration);

        private:
            SettingsProvider* settings_;
            bool storingSettings_;
            std::shared_ptr<HighlightConfiguration> highlightConfiguration_;
            boost::signals2::scoped_connection handleSettingChangedConnection_;
    };
}
