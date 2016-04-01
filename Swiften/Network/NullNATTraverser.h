/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Network/NATTraverser.h>

namespace Swift {
    class EventLoop;

    class NullNATTraverser : public NATTraverser {
        public:
            NullNATTraverser(EventLoop* eventLoop);

            std::shared_ptr<NATTraversalGetPublicIPRequest> createGetPublicIPRequest();
            std::shared_ptr<NATTraversalForwardPortRequest> createForwardPortRequest(int localPort, int publicPort);
            std::shared_ptr<NATTraversalRemovePortForwardingRequest> createRemovePortForwardingRequest(int localPort, int publicPort);

        private:
            EventLoop* eventLoop;
    };
}
