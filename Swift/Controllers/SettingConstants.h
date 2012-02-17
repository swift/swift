/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/Settings/SettingsProvider.h>

namespace Swift {
	class SettingConstants {
		public:
			static const SettingsProvider::Setting<bool> IDLE_GOES_OFFLINE;
			static const SettingsProvider::Setting<int> IDLE_TIMEOUT;
			static const SettingsProvider::Setting<bool> SHOW_NOTIFICATIONS;
			static const SettingsProvider::Setting<bool> REQUEST_DELIVERYRECEIPTS;
			static const SettingsProvider::Setting<bool> FORGET_PASSWORDS;
			static const SettingsProvider::Setting<bool> REMEMBER_RECENT_CHATS;
			static const SettingsProvider::Setting<std::string> LAST_LOGIN_JID;
			static const SettingsProvider::Setting<bool> LOGIN_AUTOMATICALLY;
			static const SettingsProvider::Setting<bool> SHOW_OFFLINE;
			static const SettingsProvider::Setting<std::string> EXPANDED_ROSTER_GROUPS;
			static const SettingsProvider::Setting<bool> PLAY_SOUNDS;
	};
}
