/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ContainerPayload.h>
#include <Swiften/Elements/Forwarded.h>

namespace Swift {
    class SWIFTEN_API MAMResult : public ContainerPayload<Forwarded> {
        public:
            virtual ~MAMResult();

            void setID(const std::string& id) { id_ = id; }
            const std::string& getID() const { return id_; }

            void setQueryID(const boost::optional<std::string>& queryID) { queryID_ = queryID; }
            const boost::optional<std::string>& getQueryID() const { return queryID_; }

        private:
            std::string id_;
            boost::optional<std::string> queryID_;
    };
}
