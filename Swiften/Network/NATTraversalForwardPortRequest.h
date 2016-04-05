/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Network/NATPortMapping.h>

namespace Swift {
    class SWIFTEN_API NATTraversalForwardPortRequest {
        public:
            virtual ~NATTraversalForwardPortRequest();

            virtual void start() = 0;
            virtual void stop() = 0;

            boost::signals2::signal<void (boost::optional<NATPortMapping>)> onResult;
    };
}
