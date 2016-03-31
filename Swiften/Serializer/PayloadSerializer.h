/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>

namespace Swift {
    class Payload;

    class SWIFTEN_API PayloadSerializer {
        public:
            virtual ~PayloadSerializer();

            virtual bool canSerialize(boost::shared_ptr<Payload>) const = 0;
            virtual std::string serialize(boost::shared_ptr<Payload>) const = 0;
    };
}
