/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/signals2.hpp>

namespace Swift {
    class URIHandler {
        public:
            URIHandler();
            virtual ~URIHandler();

            boost::signals2::signal<void (const std::string&)> onURI;
    };
}
