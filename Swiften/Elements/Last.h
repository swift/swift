/*
 * Copyright (c) 2011-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API Last : public Payload {
        public:
            Last(int seconds = 0) : seconds_(seconds) {}

            int getSeconds() const {return seconds_;}
            void setSeconds(int seconds) {seconds_ = seconds;}

        private:
            int seconds_;
    };
}
