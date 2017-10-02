/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swiften/Base/API.h>

namespace Swift {
    class Payload;

    class SWIFTEN_API PayloadSerializer {
        public:
            virtual ~PayloadSerializer();

            virtual bool canSerialize(std::shared_ptr<Payload>) const = 0;
            virtual std::string serialize(std::shared_ptr<Payload>) const = 0;
    };
}
