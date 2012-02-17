/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/SettingConstants.h>

namespace Swift {

const SettingsProvider::Setting<bool> SettingConstants::IDLE_GOES_OFFLINE = SettingsProvider::Setting<bool>("idleGoesOffline", false);
const SettingsProvider::Setting<int> SettingConstants::IDLE_TIMEOUT = SettingsProvider::Setting<int>("idleTimeout", 600);
const SettingsProvider::Setting<bool> SettingConstants::SHOW_NOTIFICATIONS = SettingsProvider::Setting<bool>("showNotifications", true);
const SettingsProvider::Setting<bool> SettingConstants::REQUEST_DELIVERYRECEIPTS = SettingsProvider::Setting<bool>("requestDeliveryReceipts", false);
const SettingsProvider::Setting<bool> SettingConstants::FORGET_PASSWORDS = SettingsProvider::Setting<bool>("forgetPasswords", false);
const SettingsProvider::Setting<bool> SettingConstants::REMEMBER_RECENT_CHATS = SettingsProvider::Setting<bool>("rememberRecentChats", true);
const SettingsProvider::Setting<std::string> SettingConstants::LAST_LOGIN_JID = SettingsProvider::Setting<std::string>("lastLoginJID", "");
const SettingsProvider::Setting<bool> SettingConstants::LOGIN_AUTOMATICALLY = SettingsProvider::Setting<bool>("loginAutomatically", false);
const SettingsProvider::Setting<bool> SettingConstants::SHOW_OFFLINE("showOffline", false);
const SettingsProvider::Setting<std::string> SettingConstants::EXPANDED_ROSTER_GROUPS("GroupExpandiness", "");
const SettingsProvider::Setting<bool> SettingConstants::PLAY_SOUNDS("playSounds", true);
}
