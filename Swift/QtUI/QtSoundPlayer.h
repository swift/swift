/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QObject>

#include <Swift/Controllers/SoundPlayer.h>

namespace Swift {
    class ApplicationPathProvider;


    class QtSoundPlayer : public QObject, public SoundPlayer {
            Q_OBJECT
        public:
            QtSoundPlayer(ApplicationPathProvider* applicationPathProvider);

            void playSound(SoundEffect sound, const std::string& soundResource);

        private:
            void playSound(const std::string& soundResource);

        private:
            ApplicationPathProvider* applicationPathProvider;
    };
}
