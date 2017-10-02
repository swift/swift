/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtUISettingConstants.h>

#include <Swift/QtUI/SwiftUpdateFeeds.h>

namespace Swift {

const SettingsProvider::Setting<bool> QtUISettingConstants::COMPACT_ROSTER("compactRoster", false);
const SettingsProvider::Setting<std::string> QtUISettingConstants::CLICKTHROUGH_BANNER("clickthroughBanner", "");
const SettingsProvider::Setting<int> QtUISettingConstants::CURRENT_ROSTER_TAB("currentRosterTab", 0);
const SettingsProvider::Setting<bool> QtUISettingConstants::SHOW_NICK_IN_ROSTER_HEADER("showNickInRosterHeader", true);
const SettingsProvider::Setting<int> QtUISettingConstants::CHATWINDOW_FONT_SIZE("chatWindowFontSize_V3", 3);
const SettingsProvider::Setting<int> QtUISettingConstants::HISTORYWINDOW_FONT_SIZE("historyWindowFontSize", 0);
const SettingsProvider::Setting<bool> QtUISettingConstants::SHOW_EMOTICONS("showEmoticons", true);
const SettingsProvider::Setting<bool> QtUISettingConstants::USE_PLAIN_CHATS("plainChats", false);
const SettingsProvider::Setting<bool> QtUISettingConstants::USE_SCREENREADER("screenreader", false);
const SettingsProvider::Setting<bool> QtUISettingConstants::SPELL_CHECKER("spellChecker", false);
const SettingsProvider::Setting<std::string> QtUISettingConstants::SPELL_CHECKER_LANGUAGE("spellCheckerLanguage", "en_US");
const SettingsProvider::Setting<std::string> QtUISettingConstants::TRELLIS_GRID_SIZE("trellisGridSize", "");
const SettingsProvider::Setting<std::string> QtUISettingConstants::TRELLIS_GRID_POSITIONS("trellisGridPositions", "");
const SettingsProvider::Setting<bool> QtUISettingConstants::ENABLE_SOFTWARE_UPDATES("enableSoftwareUpdates", true);
const SettingsProvider::Setting<std::string> QtUISettingConstants::SOFTWARE_UPDATE_CHANNEL("softwareUpdateChannel", UpdateFeeds::StableChannel);

}
