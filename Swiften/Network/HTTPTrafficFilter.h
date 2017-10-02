/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <utility>
#include <vector>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>

namespace Swift {

class SWIFTEN_API HTTPTrafficFilter {
    public:
        virtual ~HTTPTrafficFilter();
        /**
         * @brief This method is called by the HTTPConnectPRoxiedConnection on every incoming HTTP response.
         *        It can be used to insert additional HTTP requests into the HTTP CONNECT proxy initalization process.
         * @return A vector of HTTP header fields to use in a new request. If an empty vector is returned,
         *         no new request will be send and the normal proxy logic continues.
         */
        virtual std::vector<std::pair<std::string, std::string> > filterHTTPResponseHeader(const std::string& statusLine, const std::vector<std::pair<std::string, std::string> >& /* responseHeader */) = 0;
};

}
