/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API CarbonsEnable : public Payload {
        public:
            typedef std::shared_ptr<CarbonsEnable> ref;

        public:
            virtual ~CarbonsEnable();
    };
}
