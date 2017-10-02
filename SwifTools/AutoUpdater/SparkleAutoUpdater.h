/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
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
            SparkleAutoUpdater(const std::string& appcastFeed);
            ~SparkleAutoUpdater() override;

            void setAppcastFeed(const std::string& appcastFeed) override;
            void checkForUpdates() override;
            State getCurrentState() override;
            bool applicationInstallationLocationWritable() override;

        private:
            void setCurrentState(State updatedState);

        private:
            class Private;
            const std::unique_ptr<Private> d;
    };
}
