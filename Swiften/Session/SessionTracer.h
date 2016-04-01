/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Session/Session.h>

namespace Swift {
    class SWIFTEN_API SessionTracer {
        public:
            SessionTracer(std::shared_ptr<Session> session);

        private:
            void printData(char direction, const SafeByteArray& data);

            std::shared_ptr<Session> session;
    };
}
