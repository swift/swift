/*
 * Copyright (c) 2012-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/Settings/SettingsProvider.h>

namespace Swift {
    /**
     *  This class contains the major setting keys for Swift.
     */
    class SettingConstants {
        public:
            /**
             * The #IDLE_GOES_OFFLINE setting specifies whether to close the XMPP connection when
             * the user went idle.
             *
             * True for automatic close of the XMPP connection and false for only changing the presence on idle.
             */
            static const SettingsProvider::Setting<bool> IDLE_GOES_OFFLINE;
            /**
             * The #IDLE_TIMEOUT setting specifieds the seconds the user has to be inactive at the
             * desktop so the user is regarded as idle.
             */
            static const SettingsProvider::Setting<int> IDLE_TIMEOUT;
            static const SettingsProvider::Setting<bool> SHOW_NOTIFICATIONS;
            /**
             * The #REQUEST_DELIVERYRECEIPTS settings specifies whether to request delivery receipts
             * for messages to contacts that support message receipts.
             */
            static const SettingsProvider::Setting<bool> REQUEST_DELIVERYRECEIPTS;
            static const SettingsProvider::Setting<bool> FORGET_PASSWORDS;
            static const SettingsProvider::Setting<bool> REMEMBER_RECENT_CHATS;
            static const SettingsProvider::Setting<std::string> LAST_LOGIN_JID;
            static const SettingsProvider::Setting<bool> LOGIN_AUTOMATICALLY;
            /**
             * The #SHOW_OFFLINE setting specifies whether or not to show offline contacts in the
             * roster.
             *
             * If set true Swift will show offline contacts; else not.
             */
            static const SettingsProvider::Setting<bool> SHOW_OFFLINE;
            /**
             * The #EXPANDED_ROSTER_GROUPS setting specifies the list of groups that are expanded
             * in the roster UI.
             *
             * Its value is a string with group names seperated by newlines.
             */
            static const SettingsProvider::Setting<std::string> EXPANDED_ROSTER_GROUPS;
            static const SettingsProvider::Setting<bool> PLAY_SOUNDS;
            /**
             * The #HIGHLIGHT_RULES setting specifies the highlight rules and the associated actions.
             *
             * Its value is a Boost serialized representation.
             */
            static const SettingsProvider::Setting<std::string> HIGHLIGHT_RULES;
            /**
             * The #HIGHLIGHT_RULES_V2 setting specifies the second version of highlight configuration
             * rules, incompatible to old highlight rules.
             *
             * Its value is a Boost serialized representation.
             */
            static const SettingsProvider::Setting<std::string> HIGHLIGHT_RULES_V2;
            /**
             * The #INVITE_AUTO_ACCEPT_MODE setting specifies how to handle invites to chat rooms.
             *
             * Supported values are:
             * - "no" : It is up to the user whether to accept the invitation and enter a room or not.
             * - "presence" : The invitation is automatically accepted if it is from a contact that is
             *   already allowed to see the user's presence status.
             * - "domain" : The invitation is automatically accepted if it is from a contact that is
             *   already allowed to see the user's presence status or from a contact of user's domain.
             */
            static const SettingsProvider::Setting<std::string> INVITE_AUTO_ACCEPT_MODE;
            /**
             * The #DISCONNECT_ON_CARD_REMOVAL setting
             * specifies whether or not to sign out the user when
             * the smartcard is removed.
             *
             * If set true Swift will sign out the user when the
             * smart card is removed; else not.
             */
            static const SettingsProvider::Setting<bool> DISCONNECT_ON_CARD_REMOVAL;
            /**
             * The #SINGLE_SIGN_ON setting
             * specifies whether to log in using Single Sign On.
             * This is currently supported on Windows.
             *
             * If set true Swift will use GSSAPI authentication to
             * log in the user; else not.
             */
            static const SettingsProvider::Setting<bool> SINGLE_SIGN_ON;
    };
}
