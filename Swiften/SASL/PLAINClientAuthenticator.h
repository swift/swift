/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/SASL/ClientAuthenticator.h>

namespace Swift {
    class SWIFTEN_API PLAINClientAuthenticator : public ClientAuthenticator {
        public:
            PLAINClientAuthenticator();

            virtual boost::optional<SafeByteArray> getResponse() const;
            virtual bool setChallenge(const boost::optional<ByteArray>&);
    };
}
