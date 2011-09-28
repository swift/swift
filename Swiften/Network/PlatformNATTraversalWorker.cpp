/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "PlatformNATTraversalWorker.h"

#include <boost/smart_ptr/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Network/NATTraversalGetPublicIPRequest.h>
#include <Swiften/Network/NATTraversalForwardPortRequest.h>
#include <Swiften/Network/NATTraversalRemovePortForwardingRequest.h>
#include <Swiften/Network/NATPMPInterface.h>
#include <Swiften/Network/MiniUPnPInterface.h>

namespace Swift {

class PlatformNATTraversalRequest : public boost::enable_shared_from_this<PlatformNATTraversalRequest> {
	public:
		typedef boost::shared_ptr<PlatformNATTraversalRequest> ref;

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


		virtual void runBlocking() = 0;

	private:
		PlatformNATTraversalWorker* worker;
};

class PlatformNATTraversalGetPublicIPRequest : public NATTraversalGetPublicIPRequest, public PlatformNATTraversalRequest {
	public:
		PlatformNATTraversalGetPublicIPRequest(PlatformNATTraversalWorker* worker) : PlatformNATTraversalRequest(worker) {
		}

		virtual void run() {
			doRun();
		}

		virtual void runBlocking() {
			onResult(getNATTraversalInterface()->getPublicIP());
		}
};

class PlatformNATTraversalForwardPortRequest : public NATTraversalForwardPortRequest, public PlatformNATTraversalRequest {
	public:
		PlatformNATTraversalForwardPortRequest(PlatformNATTraversalWorker* worker, unsigned int localIP, unsigned int publicIP) : PlatformNATTraversalRequest(worker), localIP(localIP), publicIP(publicIP) {
		}

		virtual void run() {
			doRun();
		}

		virtual void runBlocking() {
			onResult(getNATTraversalInterface()->addPortForward(localIP, publicIP));
		}

	private:
		unsigned int localIP;
		unsigned int publicIP;
};

class PlatformNATTraversalRemovePortForwardingRequest : public NATTraversalRemovePortForwardingRequest, public PlatformNATTraversalRequest {
	public:
		PlatformNATTraversalRemovePortForwardingRequest(PlatformNATTraversalWorker* worker, const NATPortMapping& mapping) : PlatformNATTraversalRequest(worker), mapping(mapping) {
		}

		virtual void run() {
			doRun();
		}

		virtual void runBlocking() {
			onResult(getNATTraversalInterface()->removePortForward(mapping));
		}

	private:
		NATPortMapping mapping;
};

PlatformNATTraversalWorker::PlatformNATTraversalWorker(EventLoop* eventLoop) : eventLoop(eventLoop), stopRequested(false), natPMPSupported(boost::logic::indeterminate), natPMPInterface(NULL), miniUPnPSupported(boost::logic::indeterminate), miniUPnPInterface(NULL) {
	nullNATTraversalInterface = new NullNATTraversalInterface();
	thread = new boost::thread(boost::bind(&PlatformNATTraversalWorker::run, this));
}

PlatformNATTraversalWorker::~PlatformNATTraversalWorker() {
	stopRequested = true;
	addRequestToQueue(boost::shared_ptr<PlatformNATTraversalRequest>());
	thread->join();
	delete thread;
	delete natPMPInterface;
	delete miniUPnPInterface;
	delete nullNATTraversalInterface;
}

NATTraversalInterface* PlatformNATTraversalWorker::getNATTraversalInterface() const {
	if (boost::logic::indeterminate(miniUPnPSupported)) {
		miniUPnPInterface = new MiniUPnPInterface();
		miniUPnPSupported = miniUPnPInterface->isAvailable();
	}
	if (miniUPnPSupported) {
		return miniUPnPInterface;
	}


	if (boost::logic::indeterminate(natPMPSupported)) {
		natPMPInterface = new NATPMPInterface();
		natPMPSupported = natPMPInterface->isAvailable();
	}
	if (natPMPSupported) {
		return natPMPInterface;
	}

	return nullNATTraversalInterface;
}

boost::shared_ptr<NATTraversalGetPublicIPRequest> PlatformNATTraversalWorker::createGetPublicIPRequest() {
	return boost::make_shared<PlatformNATTraversalGetPublicIPRequest>(this);
}

boost::shared_ptr<NATTraversalForwardPortRequest> PlatformNATTraversalWorker::createForwardPortRequest(int localPort, int publicPort) {
	return boost::make_shared<PlatformNATTraversalForwardPortRequest>(this, localPort, publicPort);
}

boost::shared_ptr<NATTraversalRemovePortForwardingRequest> PlatformNATTraversalWorker::createRemovePortForwardingRequest(int localPort, int publicPort) {
	NATPortMapping mapping(localPort, publicPort, NATPortMapping::TCP); // FIXME
	return boost::make_shared<PlatformNATTraversalRemovePortForwardingRequest>(this, mapping);
}

void PlatformNATTraversalWorker::run() {
	while (!stopRequested) {
		PlatformNATTraversalRequest::ref request;
		{
			boost::unique_lock<boost::mutex> lock(queueMutex);
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

void PlatformNATTraversalWorker::addRequestToQueue(PlatformNATTraversalRequest::ref request) {
	{
		boost::lock_guard<boost::mutex> lock(queueMutex);
		queue.push_back(request);
	}
	queueNonEmpty.notify_one();
}

}
