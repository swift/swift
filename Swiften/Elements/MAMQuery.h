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
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/ResultSet.h>

namespace Swift {
    class SWIFTEN_API MAMQuery : public Payload {
        public:
            virtual ~MAMQuery();

            void setQueryID(const boost::optional<std::string>& queryID) { queryID_ = queryID; }
            const boost::optional<std::string>& getQueryID() const { return queryID_; }

            void setNode(const boost::optional<std::string>& node) { node_ = node; }
            const boost::optional<std::string>& getNode() const { return node_; }

            void setForm(std::shared_ptr<Form> form) { form_ = form; }
            const std::shared_ptr<Form>& getForm() const { return form_; }

            void setResultSet(std::shared_ptr<ResultSet> resultSet) { resultSet_ = resultSet; }
            const std::shared_ptr<ResultSet>& getResultSet() const { return resultSet_; }

        private:
            boost::optional<std::string> queryID_;
            boost::optional<std::string> node_;
            std::shared_ptr<Form> form_;
            std::shared_ptr<ResultSet> resultSet_;
    };
}
