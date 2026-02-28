/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
    class JID;

    class UserRegistry {
        public:
            virtual ~UserRegistry();

            virtual bool isValidUserPassword(const JID& user, const SafeByteArray& password) const = 0;
    };
}
