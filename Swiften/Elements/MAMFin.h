/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/ResultSet.h>

namespace Swift {
    class SWIFTEN_API MAMFin : public Payload {
        public:
            MAMFin() : isComplete_(false), isStable_(true) {}
            virtual ~MAMFin();

            void setComplete(const bool isComplete) {
                isComplete_ = isComplete;
            }

            bool isComplete() const {
                return isComplete_;
            }

            void setStable(const bool isStable) {
                isStable_ = isStable;
            }

            bool isStable() const {
                return isStable_;
            }

            void setResultSet(std::shared_ptr<ResultSet> resultSet) {
                resultSet_ = resultSet;
            }

            std::shared_ptr<ResultSet> getResultSet() const {
                return resultSet_;
            }

            void setQueryID(const std::string& queryID) {
                queryID_ = queryID;
            }

            const boost::optional<std::string>& getQueryID() const {
                return queryID_;
            }


        private:
            bool isComplete_;
            bool isStable_;
            std::shared_ptr<ResultSet> resultSet_;
            boost::optional<std::string> queryID_;
    };
}
