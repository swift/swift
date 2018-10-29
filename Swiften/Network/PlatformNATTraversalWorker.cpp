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

#include <Swiften/Network/PlatformNATTraversalWorker.h>

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/Network/NATTraversalGetPublicIPRequest.h>
#include <Swiften/Network/NATTraversalForwardPortRequest.h>
#include <Swiften/Network/NATTraversalRemovePortForwardingRequest.h>
#ifdef HAVE_LIBNATPMP
#include <Swiften/Network/NATPMPInterface.h>
#endif
#ifdef HAVE_LIBMINIUPNPC
#include <Swiften/Network/MiniUPnPInterface.h>
#endif

namespace Swift {

class PlatformNATTraversalRequest : public std::enable_shared_from_this<PlatformNATTraversalRequest>, public EventOwner {
    public:
        typedef std::shared_ptr<PlatformNATTraversalRequest> ref;

    public:
        PlatformNATTraversalRequest(PlatformNATTraversalWorker* worker) : worker(worker) {
        }

        virtual ~PlatformNATTraversalRequest() {
        }

        virtual void doRun() {
            worker->addRequestToQueue(shared_from_this());
        }

        NATTraversalInterface* getNATTraversalInterface() const {
            return worker->getNATTraversalInterface();
        }

        EventLoop* getEventLoop() const {
            return worker->getEventLoop();
        }


        virtual void runBlocking() = 0;

    private:
        PlatformNATTraversalWorker* worker;
};

class PlatformNATTraversalGetPublicIPRequest : public NATTraversalGetPublicIPRequest, public PlatformNATTraversalRequest {
    public:
        PlatformNATTraversalGetPublicIPRequest(PlatformNATTraversalWorker* worker) : PlatformNATTraversalRequest(worker) {
        }

        virtual ~PlatformNATTraversalGetPublicIPRequest() {
        }

        virtual void start() {
            doRun();
        }

        virtual void stop() {
            onResult.disconnect_all_slots();
        }

        virtual void runBlocking() {
            getEventLoop()->postEvent(boost::bind(boost::ref(onResult), getNATTraversalInterface()->getPublicIP()), shared_from_this());
        }
};

class PlatformNATTraversalForwardPortRequest : public NATTraversalForwardPortRequest, public PlatformNATTraversalRequest {
    public:
        PlatformNATTraversalForwardPortRequest(PlatformNATTraversalWorker* worker, unsigned short localPort, unsigned short publicPort) : PlatformNATTraversalRequest(worker), localPort(localPort), publicPort(publicPort) {
        }

        virtual ~PlatformNATTraversalForwardPortRequest() {
        }

        virtual void start() {
            doRun();
        }

        virtual void stop() {
            onResult.disconnect_all_slots();
        }

        virtual void runBlocking() {
            getEventLoop()->postEvent(boost::bind(boost::ref(onResult), getNATTraversalInterface()->addPortForward(localPort, publicPort)), shared_from_this());
        }

    private:
        unsigned short localPort;
        unsigned short publicPort;
};

class PlatformNATTraversalRemovePortForwardingRequest : public NATTraversalRemovePortForwardingRequest, public PlatformNATTraversalRequest {
    public:
        PlatformNATTraversalRemovePortForwardingRequest(PlatformNATTraversalWorker* worker, const NATPortMapping& mapping) : PlatformNATTraversalRequest(worker), mapping(mapping) {
        }

        virtual ~PlatformNATTraversalRemovePortForwardingRequest() {
        }

        virtual void start() {
            doRun();
        }

        virtual void stop() {
            onResult.disconnect_all_slots();
        }

        virtual void runBlocking() {
            getEventLoop()->postEvent(boost::bind(boost::ref(onResult), getNATTraversalInterface()->removePortForward(mapping)), shared_from_this());
        }

    private:
        NATPortMapping mapping;
};

PlatformNATTraversalWorker::PlatformNATTraversalWorker(EventLoop* eventLoop) : eventLoop(eventLoop), stopRequested(false), natPMPSupported(boost::logic::indeterminate), natPMPInterface(nullptr), miniUPnPSupported(boost::logic::indeterminate), miniUPnPInterface(nullptr) {
    nullNATTraversalInterface = new NullNATTraversalInterface();
    // FIXME: This should be done from start(), and the current start() should be an internal method
    thread = new std::thread(boost::bind(&PlatformNATTraversalWorker::start, this));
}

PlatformNATTraversalWorker::~PlatformNATTraversalWorker() {
    stopRequested = true;
    addRequestToQueue(std::shared_ptr<PlatformNATTraversalRequest>());
    thread->join();
    delete thread;
#ifdef HAVE_LIBNATPMP
    delete natPMPInterface;
#endif
#ifdef HAVE_LIBMINIUPNPC
    delete miniUPnPInterface;
#endif
    delete nullNATTraversalInterface;
}

NATTraversalInterface* PlatformNATTraversalWorker::getNATTraversalInterface() const {
#ifdef HAVE_LIBMINIUPNPC
    if (boost::logic::indeterminate(miniUPnPSupported)) {
        miniUPnPInterface = new MiniUPnPInterface();
        miniUPnPSupported = miniUPnPInterface->isAvailable();
    }
    SWIFT_LOG(debug) << "UPnP NAT traversal supported: " << miniUPnPSupported << std::endl;
    if (miniUPnPSupported) {
        return miniUPnPInterface;
    }
#endif

#ifdef HAVE_LIBNATPMP
    if (boost::logic::indeterminate(natPMPSupported)) {
        natPMPInterface = new NATPMPInterface();
        natPMPSupported = natPMPInterface->isAvailable();
    }
    SWIFT_LOG(debug) << "NAT-PMP NAT traversal supported: " << natPMPSupported << std::endl;
    if (natPMPSupported) {
        return natPMPInterface;
    }
#endif

    return nullNATTraversalInterface;
}

std::shared_ptr<NATTraversalGetPublicIPRequest> PlatformNATTraversalWorker::createGetPublicIPRequest() {
    return std::make_shared<PlatformNATTraversalGetPublicIPRequest>(this);
}

std::shared_ptr<NATTraversalForwardPortRequest> PlatformNATTraversalWorker::createForwardPortRequest(unsigned short localPort, unsigned short publicPort) {
    return std::make_shared<PlatformNATTraversalForwardPortRequest>(this, localPort, publicPort);
}

std::shared_ptr<NATTraversalRemovePortForwardingRequest> PlatformNATTraversalWorker::createRemovePortForwardingRequest(unsigned short localPort, unsigned short publicPort) {
    NATPortMapping mapping(localPort, publicPort, NATPortMapping::TCP); // FIXME
    return std::make_shared<PlatformNATTraversalRemovePortForwardingRequest>(this, mapping);
}

void PlatformNATTraversalWorker::start() {
    while (!stopRequested) {
        PlatformNATTraversalRequest::ref request;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            while (queue.empty()) {
                queueNonEmpty.wait(lock);
            }
            request = queue.front();
            queue.pop_front();
        }
        // Check whether we don't have a non-null request (used to stop the
        // worker)
        if (request) {
            request->runBlocking();
        }
    }
}

void PlatformNATTraversalWorker::stop() {
    // TODO
}

void PlatformNATTraversalWorker::addRequestToQueue(PlatformNATTraversalRequest::ref request) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        queue.push_back(request);
    }
    queueNonEmpty.notify_one();
}

}
