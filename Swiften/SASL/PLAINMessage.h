/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

// TODO: Get rid of this
//
#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
    class SWIFTEN_API PLAINMessage {
        public:
            PLAINMessage(const std::string& authcid, const SafeByteArray& password, const std::string& authzid = "");
            PLAINMessage(const SafeByteArray& value);

            SafeByteArray getValue() const;

            const std::string& getAuthenticationID() const {
                return authcid;
            }

            const SafeByteArray& getPassword() const {
                return password;
            }

            const std::string& getAuthorizationID() const {
                return authzid;
            }

        private:
            std::string authcid;
            std::string authzid;
            SafeByteArray password;
    };
}
