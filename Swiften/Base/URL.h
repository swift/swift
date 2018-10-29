/*
 * Copyright (c) 2011-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/API.h>

namespace Swift {

class SWIFTEN_API URL {
    public:

        URL() : scheme(""), user(""), password(""), host(""), path(""), empty(true) {
        }

        URL(const std::string& scheme, const std::string& host, unsigned short port, const std::string& path) : scheme(scheme), user(), password(), host(host), port(port), path(path), empty(false) {
        }

        URL(const std::string& scheme, const std::string& host, const std::string& path) : scheme(scheme), user(), password(), host(host), path(path), empty(false) {
        }

        /**
         * Whether the URL is empty.
         */
        bool isEmpty() const {
            return empty;
        }

        /**
         * Scheme used for the URL (http, https etc.)
         */
        const std::string& getScheme() const {
            return scheme;
        }

        /**
         * Hostname
         */
        const std::string& getHost() const {
            return host;
        }

        /**
         * Port number
         */
        boost::optional<unsigned short> getPort() const {
            return port;
        }

        /**
         * Path
         */
        const std::string& getPath() const {
            return path;
        }

        std::string toString() const;

        static unsigned short getPortOrDefaultPort(const URL& url);
        static URL fromString(const std::string&);
        static std::string unescape(const std::string&);


    private:
        std::string scheme;
        std::string user;
        std::string password;
        std::string host;
        boost::optional<unsigned short> port;
        std::string path;
        bool empty;
    };
}
