/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <boost/filesystem.hpp>

namespace Swift {
    class ApplicationPathProvider {
        public:
            ApplicationPathProvider(const std::string& applicationName);
            virtual ~ApplicationPathProvider();

            virtual boost::filesystem::path getHomeDir() const = 0;
            virtual boost::filesystem::path getDataDir() const = 0;
            boost::filesystem::path getExecutableDir() const;
            boost::filesystem::path getProfileDir(const std::string& profile) const;
            boost::filesystem::path getResourcePath(const std::string& resource) const;

        protected:
            virtual std::vector<boost::filesystem::path> getResourceDirs() const = 0;
            const std::string& getApplicationName() const {
                return applicationName;
            }

        private:
            std::string applicationName;
    };
}
