/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/SoundPlayer.h>

namespace Swift {
    class DummySoundPlayer : public SoundPlayer {
        public:
            void playSound(SoundEffect /*sound*/, const std::string& /*soundResource*/) {}
    };
}
