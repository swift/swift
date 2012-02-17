/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/QtUI/QtUISettingConstants.h>

namespace Swift {

const SettingsProvider::Setting<bool> QtUISettingConstants::COMPACT_ROSTER("compactRoster", false);
const SettingsProvider::Setting<std::string> QtUISettingConstants::CLICKTHROUGH_BANNER("clickthroughBanner", "");
const SettingsProvider::Setting<int> QtUISettingConstants::CURRENT_ROSTER_TAB("currentRosterTab", 0);
const SettingsProvider::Setting<bool> QtUISettingConstants::SHOW_NICK_IN_ROSTER_HEADER("showNickInRosterHeader", true);
const SettingsProvider::Setting<int> QtUISettingConstants::CHATWINDOW_FONT_SIZE("chatWindowFontSize", 0);
}
