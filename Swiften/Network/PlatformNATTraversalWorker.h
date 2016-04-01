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

#include <deque>

#include <boost/logic/tribool.hpp>
#include <boost/optional.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Atomic.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/NATTraverser.h>
#include <Swiften/Network/NullNATTraversalInterface.h>

namespace Swift {
    class EventLoop;
    class NATTraversalGetPublicIPRequest;
    class NATTraversalForwardPortRequest;
    class NATTraversalRemovePortForwardingRequest;
    class PlatformNATTraversalRequest;
    class NATPMPInterface;
    class MiniUPnPInterface;
    class NATTraversalInterface;
    class NATPortMapping;

    class SWIFTEN_API PlatformNATTraversalWorker : public NATTraverser {
            friend class PlatformNATTraversalRequest;

        public:
            PlatformNATTraversalWorker(EventLoop* eventLoop);
            virtual ~PlatformNATTraversalWorker();

            std::shared_ptr<NATTraversalGetPublicIPRequest> createGetPublicIPRequest();
            std::shared_ptr<NATTraversalForwardPortRequest> createForwardPortRequest(int localPort, int publicPort);
            std::shared_ptr<NATTraversalRemovePortForwardingRequest> createRemovePortForwardingRequest(int localPort, int publicPort);

        private:
            NATTraversalInterface* getNATTraversalInterface() const;
            void addRequestToQueue(std::shared_ptr<PlatformNATTraversalRequest>);
            void start();
            void stop();

            EventLoop* getEventLoop() const {
                return eventLoop;
            }

        private:
            EventLoop* eventLoop;
            Atomic<bool> stopRequested;
            boost::thread* thread;
            std::deque<std::shared_ptr<PlatformNATTraversalRequest> > queue;
            boost::mutex queueMutex;
            boost::condition_variable queueNonEmpty;

            NullNATTraversalInterface* nullNATTraversalInterface;
            mutable boost::logic::tribool natPMPSupported;
            mutable NATPMPInterface* natPMPInterface;
            mutable boost::logic::tribool miniUPnPSupported;
            mutable MiniUPnPInterface* miniUPnPInterface;
    };
}
