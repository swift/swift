/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

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
            static const SettingsProvider::Setting<bool> SPELL_CHECKER;
            static const SettingsProvider::Setting<std::string> SPELL_CHECKER_LANGUAGE;
            /**
             * The #TRELLIS_GRID_SIZE setting specifies the dimensions of the grid used for the trellis
             * layout.
             *
             * Its value is a Qt serialized representation.
             */
            static const SettingsProvider::Setting<std::string> TRELLIS_GRID_SIZE;
            /**
             * The #TRELLIS_GRID_POSITIONS setting specifies where conversations to contacts or rooms go
             * in the trellis grid.
             *
             * Its value is a Qt serialized representation.
             */
            static const SettingsProvider::Setting<std::string> TRELLIS_GRID_POSITIONS;
            /**
             * The #ENABLE_SOFTWARE_UPDATES setting specifies, whether Swift
             * should automatically check for software updates in regular
             * intervals and install them automatically.
             */
            static const SettingsProvider::Setting<bool> ENABLE_SOFTWARE_UPDATES;
            /**
             * The #SOFTWARE_UPDATE_CHANNEL setting defines what update channel
             * Swift uses to check for, and receive, updates.
             */
            static const SettingsProvider::Setting<std::string> SOFTWARE_UPDATE_CHANNEL;
    };
}
