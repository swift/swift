/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <SwifTools/AutoUpdater/AutoUpdater.h>

namespace Swift {
    class SparkleAutoUpdater : public AutoUpdater {
        public:
            SparkleAutoUpdater(const std::string& url);
            ~SparkleAutoUpdater();

            void checkForUpdates();

        private:
            class Private;
            Private* d;
    };
}
