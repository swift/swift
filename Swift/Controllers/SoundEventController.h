/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swift/Controllers/Highlighting/HighlightAction.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swift/Controllers/XMPPEvents/StanzaEvent.h>

namespace Swift {
    class EventController;
    class SoundPlayer;
    class HighlightManager;
    class SoundEventController {
        public:
            SoundEventController(EventController* eventController, SoundPlayer* soundPlayer, SettingsProvider* settings, HighlightManager* highlightManager);
            void setPlaySounds(bool playSounds);
            bool getSoundEnabled() {return playSounds_;}

        private:
            void handleSettingChanged(const std::string& settingPath);
            void handleEventQueueEventAdded(std::shared_ptr<StanzaEvent> event);
            void handleHighlight(const HighlightAction& action);

        private:
            EventController* eventController_;
            SoundPlayer* soundPlayer_;
            bool playSounds_;
            SettingsProvider* settings_;
            HighlightManager* highlightManager_;
    };
}
