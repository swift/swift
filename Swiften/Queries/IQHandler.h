/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/IQ.h>

namespace Swift {
    class SWIFTEN_API IQHandler {
        public:
            virtual ~IQHandler();

            virtual bool handleIQ(std::shared_ptr<IQ>) = 0;
    };
}
