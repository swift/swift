/*
 * Copyright (c) 2011-2018 Isode Limited.
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
            std::shared_ptr<NATTraversalForwardPortRequest> createForwardPortRequest(unsigned short localPort, unsigned short publicPort);
            std::shared_ptr<NATTraversalRemovePortForwardingRequest> createRemovePortForwardingRequest(unsigned short localPort, unsigned short publicPort);

        private:
            EventLoop* eventLoop;
    };
}
