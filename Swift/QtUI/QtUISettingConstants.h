/*
 * Copyright (c) 2012-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/Settings/SettingsProvider.h>

namespace Swift {
	class QtUISettingConstants {
		public:
			static const SettingsProvider::Setting<bool> COMPACT_ROSTER;
			static const SettingsProvider::Setting<std::string> CLICKTHROUGH_BANNER;
			static const SettingsProvider::Setting<int> CURRENT_ROSTER_TAB;
			static const SettingsProvider::Setting<bool> SHOW_NICK_IN_ROSTER_HEADER;
			static const SettingsProvider::Setting<int> CHATWINDOW_FONT_SIZE;
			static const SettingsProvider::Setting<int> HISTORYWINDOW_FONT_SIZE;
			static const SettingsProvider::Setting<bool> SHOW_EMOTICONS;
			static const SettingsProvider::Setting<bool> USE_PLAIN_CHATS;
			static const SettingsProvider::Setting<bool> USE_SCREENREADER;
	};
}
