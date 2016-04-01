/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>

namespace Swift {
    class Connection;

    class SWIFTEN_API ConnectionFactory {
        public:
            virtual ~ConnectionFactory();

            virtual std::shared_ptr<Connection> createConnection() = 0;
    };
}
