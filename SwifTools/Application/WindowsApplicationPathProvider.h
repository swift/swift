/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <SwifTools/Application/ApplicationPathProvider.h>

namespace Swift {
    class WindowsApplicationPathProvider : public ApplicationPathProvider {
        public:
            WindowsApplicationPathProvider(const std::string& name);

            boost::filesystem::path getDataDir() const;
            boost::filesystem::path getHomeDir() const;

            virtual std::vector<boost::filesystem::path> getResourceDirs() const {
                return resourceDirs;
            }

        private:
            std::vector<boost::filesystem::path> resourceDirs;
    };
}
