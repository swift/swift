/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <string>

namespace Swift {
    class AutoUpdater;

    class PlatformAutoUpdaterFactory {
        public:
            bool isSupported() const;

            AutoUpdater* createAutoUpdater(const std::string& appcastURL);
    };
}
