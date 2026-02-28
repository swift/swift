/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

namespace Swift {
    class SoundPlayer {
        public:
            virtual ~SoundPlayer() {}
            enum SoundEffect{MessageReceived};
            virtual void playSound(SoundEffect sound, const std::string& soundResource) = 0;
    };
}
