/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API Priority : public Payload {
        public:
            Priority(int priority = 0) : priority_(priority) {
            }

            void setPriority(int priority) {
                priority_ = priority;
            }

            int getPriority() const {
                return priority_;
            }

        private:
            int priority_;
    };
}
