/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Network/HostAddress.h>

namespace Swift {
    class SWIFTEN_API NATTraversalGetPublicIPRequest {
        public:
            virtual ~NATTraversalGetPublicIPRequest();

            virtual void start() = 0;
            virtual void stop() = 0;

            boost::signal<void (boost::optional<HostAddress>)> onResult;
    };
}
