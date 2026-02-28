/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/signals2.hpp>

namespace Swift {
    class AutoUpdater {
        public:
            enum class State {
                NotCheckedForUpdatesYet,
                NoUpdateAvailable,
                CheckingForUpdate,
                ErrorCheckingForUpdate,
                DownloadingUpdate,
                RestartToInstallUpdate
            };

        public:
            virtual ~AutoUpdater();

            virtual void setAppcastFeed(const std::string& appcastFeed) = 0;
            virtual void checkForUpdates() = 0;
            virtual State getCurrentState() = 0;
            virtual bool applicationInstallationLocationWritable() = 0;

        public:
            /**
             * Emit this signal if a new version of the software has been downloaded
             * and the user needs to be notified so they can quit the app and start
             * the newer version.
             */
            boost::signals2::signal<void(State)> onUpdateStateChanged;
    };
}
