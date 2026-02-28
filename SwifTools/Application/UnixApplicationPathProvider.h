/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <SwifTools/Application/ApplicationPathProvider.h>

namespace Swift {
    class UnixApplicationPathProvider : public ApplicationPathProvider {
        public:
            UnixApplicationPathProvider(const std::string& name);

            virtual boost::filesystem::path getHomeDir() const;
            boost::filesystem::path getDataDir() const;

            virtual std::vector<boost::filesystem::path> getResourceDirs() const {
                return resourceDirs;
            }

        private:
            std::vector<boost::filesystem::path> resourceDirs;
    };
}

