/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>

namespace Swift {
    class NATTraversalGetPublicIPRequest;
    class NATTraversalForwardPortRequest;
    class NATTraversalRemovePortForwardingRequest;

    class SWIFTEN_API NATTraverser {
        public:
            virtual ~NATTraverser();

            virtual std::shared_ptr<NATTraversalGetPublicIPRequest> createGetPublicIPRequest() = 0;
            virtual std::shared_ptr<NATTraversalForwardPortRequest> createForwardPortRequest(int localPort, int publicPort) = 0;
            virtual std::shared_ptr<NATTraversalRemovePortForwardingRequest> createRemovePortForwardingRequest(int localPort, int publicPort) = 0;
    };
}
