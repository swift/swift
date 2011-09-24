/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "PlatformNATTraversalWorker.h"

#include <boost/smart_ptr/make_shared.hpp>
#include <Swiften/Base/Log.h>
#include <Swiften/Network/UPnPNATTraversalGetPublicIPRequest.h>
#include <Swiften/Network/UPnPNATTraversalForwardPortRequest.h>
#include <Swiften/Network/UPnPNATTraversalRemovePortForwardingRequest.h>
#include <Swiften/Network/NATPMPNATTraversalGetPublicIPRequest.h>
#include <Swiften/Network/NATPMPNATTraversalForwardPortRequest.h>
#include <Swiften/Network/NATPMPNATTraversalRemovePortForwardingRequest.h>

namespace Swift {

PlatformNATTraversalWorker::PlatformNATTraversalWorker(EventLoop* eventLoop) : backendType(NotYetDecided), eventLoop(eventLoop), stopRequested(false) {
	checkAvailableNATTraversalProtocols();
	thread = new boost::thread(boost::bind(&PlatformNATTraversalWorker::run, this));
}

PlatformNATTraversalWorker::~PlatformNATTraversalWorker() {
	stopRequested = true;
	addRequestToQueue(boost::shared_ptr<PlatformNATTraversalRequest>());
	thread->join();
	delete thread;
}

boost::shared_ptr<NATTraversalGetPublicIPRequest> PlatformNATTraversalWorker::createGetPublicIPRequest() {
	switch(backendType) {
		case UPnP:
			return boost::make_shared<UPnPNATTraversalGetPublicIPRequest>(this);
		case NATPMP:
			return boost::make_shared<NATPMPNATTraversalGetPublicIPRequest>(this);
		case NotYetDecided:
		case None:
			break;
	}
	return boost::shared_ptr<NATTraversalGetPublicIPRequest>();
}

boost::shared_ptr<NATTraversalForwardPortRequest> PlatformNATTraversalWorker::createForwardPortRequest(unsigned int localPort, unsigned int publicPort) {
	NATTraversalForwardPortRequest::PortMapping mapping;
	mapping.protocol = NATTraversalForwardPortRequest::PortMapping::TCP;
	mapping.leaseInSeconds = 60 * 60 * 24;
	mapping.localPort = localPort;
	mapping.publicPort = publicPort;

	switch(backendType) {
		case UPnP:
			return boost::make_shared<UPnPNATTraversalForwardPortRequest>(mapping, this);
		case NATPMP:
			return boost::make_shared<NATPMPNATTraversalForwardPortRequest>(mapping, this);
		case NotYetDecided:
		case None:
			break;
	}
	return boost::shared_ptr<NATTraversalForwardPortRequest>();
}

boost::shared_ptr<NATTraversalRemovePortForwardingRequest> PlatformNATTraversalWorker::createRemovePortForwardingRequest(unsigned int localPort, unsigned int publicPort) {
	NATTraversalRemovePortForwardingRequest::PortMapping mapping;
	mapping.protocol = NATTraversalRemovePortForwardingRequest::PortMapping::TCP;
	mapping.leaseInSeconds = 60 * 60 * 24;
	mapping.localPort = localPort;
	mapping.publicPort = publicPort;

	switch(backendType) {
		case UPnP:
			return boost::make_shared<UPnPNATTraversalRemovePortForwardingRequest>(mapping, this);
		case NATPMP:
			return boost::make_shared<NATPMPNATTraversalRemovePortForwardingRequest>(mapping, this);
		case NotYetDecided:
		case None:
			break;
	}
	return boost::shared_ptr<NATTraversalRemovePortForwardingRequest>();
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

void PlatformNATTraversalWorker::checkAvailableNATTraversalProtocols() {
	boost::shared_ptr<UPnPNATTraversalGetPublicIPRequest> upnpRequest = boost::make_shared<UPnPNATTraversalGetPublicIPRequest>(this);
	upnpRequest->onResult.connect(boost::bind(&PlatformNATTraversalWorker::handleUPnPGetPublicIPResult, this, _1));

	boost::shared_ptr<NATPMPNATTraversalGetPublicIPRequest> natpmpRequest = boost::make_shared<NATPMPNATTraversalGetPublicIPRequest>(this);
	natpmpRequest->onResult.connect(boost::bind(&PlatformNATTraversalWorker::handleNATPMPGetPublicIPResult, this, _1));

	upnpRequest->run();
	natpmpRequest->run();
}

void PlatformNATTraversalWorker::handleUPnPGetPublicIPResult(boost::optional<HostAddress> address) {
	if (backendType == NotYetDecided || backendType == None) {
		if (address) {
			SWIFT_LOG(debug) << "Found UPnP IGD in the local network." << std::endl;
			backendType = UPnP;
		}
	}
}

void PlatformNATTraversalWorker::handleNATPMPGetPublicIPResult(boost::optional<HostAddress> address) {
	if (backendType == NotYetDecided || backendType == None) {
		if (address) {
			SWIFT_LOG(debug) << "Found NAT-PMP device in the local network." << std::endl;
			backendType = NATPMP;
		}
	}
}

}
