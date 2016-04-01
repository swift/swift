/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/filesystem/path.hpp>

#include <Swiften/Roster/RosterStorage.h>

namespace Swift {
    class RosterFileStorage : public RosterStorage {
        public:
            RosterFileStorage(const boost::filesystem::path& path);

            virtual std::shared_ptr<RosterPayload> getRoster() const;
            virtual void setRoster(std::shared_ptr<RosterPayload>);

        private:
            boost::filesystem::path path;
    };
}
