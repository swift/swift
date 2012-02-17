/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
	};
}
