/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <SwifTools/AutoUpdater/AutoUpdater.h>

namespace Swift {
    /**
     * @brief The SparkleAutoUpdater class provides integration with Sparkle.
     * This enables automatic silent background updates. If using this in Qt you
     * need to emit a NSApplicationWillTerminateNotification before you quit
     * the application.
     */
    class SparkleAutoUpdater : public AutoUpdater {
        public:
            SparkleAutoUpdater(const std::string& url);
            ~SparkleAutoUpdater();

            void checkForUpdates();
            bool recommendRestartToUpdate();

        private:
            class Private;
            Private* d;
    };
}
