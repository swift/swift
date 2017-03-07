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

#include <Swift/Controllers/Highlighting/HighlightManager.h>

#include <cassert>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/bind.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/regex.hpp>
#include <boost/serialization/vector.hpp>

#include <Swiften/Base/Log.h>

#include <Swift/Controllers/Highlighting/HighlightConfiguration.h>
#include <Swift/Controllers/Highlighting/Highlighter.h>
#include <Swift/Controllers/SettingConstants.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>

namespace Swift {

HighlightManager::HighlightManager(SettingsProvider* settings)
    : settings_(settings)
    , storingSettings_(false) {
    highlightConfiguration_ = std::make_shared<HighlightConfiguration>();
    loadSettings();
    handleSettingChangedConnection_ = settings_->onSettingChanged.connect(boost::bind(&HighlightManager::handleSettingChanged, this, _1));
}

void HighlightManager::handleSettingChanged(const std::string& settingPath) {
    if (!storingSettings_ && SettingConstants::HIGHLIGHT_RULES.getKey() == settingPath) {
        loadSettings();
    }
}

HighlightConfiguration HighlightManager::getDefaultConfig() {
    HighlightConfiguration defaultConfiguration;
    defaultConfiguration.playSoundOnIncomingDirectMessages = true;
    defaultConfiguration.showNotificationOnIncomingDirectMessages = true;
    defaultConfiguration.ownMentionAction.setSoundFilePath(std::string("/sounds/message-received.wav"));
    defaultConfiguration.ownMentionAction.setSystemNotificationEnabled(true);
    return defaultConfiguration;
}

void HighlightManager::storeSettings() {
    storingSettings_ = true;    // don't reload settings while saving
    settings_->storeSetting(SettingConstants::HIGHLIGHT_RULES_V2, highlightConfigurationToString(*highlightConfiguration_));
    storingSettings_ = false;
}

void HighlightManager::loadSettings() {
    std::string configString = settings_->getSetting(SettingConstants::HIGHLIGHT_RULES_V2);
    *highlightConfiguration_ = highlightConfigurationFromString(configString);
}

Highlighter* HighlightManager::createHighlighter(NickResolver* nickResolver) {
    return new Highlighter(this, nickResolver);
}

void HighlightManager::resetToDefaultConfiguration() {
    *highlightConfiguration_ = getDefaultConfig();
}

HighlightConfiguration HighlightManager::highlightConfigurationFromString(const std::string& dataString) {
    std::stringstream stream;
    stream << dataString;

    HighlightConfiguration configuration;
    try {
        boost::archive::text_iarchive archive(stream);
        archive >> configuration;
    }
    catch (boost::archive::archive_exception&) {
        configuration = getDefaultConfig();
        SWIFT_LOG(warning) << "Failed to load highlight configuration. Will use default configuration instead." << std::endl;
    }
    return configuration;
}

std::string HighlightManager::highlightConfigurationToString(const HighlightConfiguration& configuration) {
    std::stringstream stream;
    boost::archive::text_oarchive archive(stream);
    archive & configuration;
    return stream.str();
}

}
