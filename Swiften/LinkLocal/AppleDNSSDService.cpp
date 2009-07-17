#include "Swiften/LinkLocal/AppleDNSSDService.h"

#include <algorithm>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>

#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"

namespace Swift {

#if 0
namespace {
	void handleServiceResolved( DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const unsigned char *txtRecord, void *context ) {
		std::cerr << "Service resolved " << fullname << " " << hosttarget << " " << port << " " << txtLen << " " << /*std::string((const char*) txtRecord, txtLen) <<*/ std::endl;
	}

	void handleAddressInfoReceived ( DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *hostname, const struct sockaddr *address, uint32_t ttl, void *context ) {
		//std::cerr << "Address received " << HostAddress((const unsigned char*) address->sa_data, 4).toString() << std::endl;
	} 
}

/*
		result = DNSServiceResolve(&resolveSDRef, 0, 6, "Remko@Micro", "_presence._tcp.", "local.", handleServiceResolved , 0);
		result = DNSServiceGetAddrInfo(&addressSDRef, 0, 6, kDNSServiceProtocol_IPv4, "Micro.local.", handleAddressInfoReceived, 0);
	*/
#endif


AppleDNSSDService::AppleDNSSDService() : thread(0), stopRequested(false), browseSDRef(0), registerSDRef(0) {
	int fds[2];
	int result = pipe(fds);
	assert(result == 0);
	interruptSelectReadSocket = fds[0];
	interruptSelectWriteSocket = fds[1];
}

AppleDNSSDService::~AppleDNSSDService() {
	stop();
}

void AppleDNSSDService::start() {
	assert(!thread);
	thread = new boost::thread(boost::bind(&AppleDNSSDService::doStart, shared_from_this()));
}

void AppleDNSSDService::registerService(const String& name, int port, const LinkLocalServiceInfo& info) {
	boost::lock_guard<boost::mutex> lock(sdRefsMutex);

	assert(!registerSDRef);
	ByteArray txtRecord = info.toTXTRecord();
	DNSServiceErrorType result = DNSServiceRegister(&registerSDRef, 0, 0, name.getUTF8Data(), "_presence._tcp", NULL, NULL, port, txtRecord.getSize(), txtRecord.getData(), &AppleDNSSDService::handleServiceRegisteredGlobal, this);
	interruptSelect();
	if (result != kDNSServiceErr_NoError) {
		onError();
	}
}

void AppleDNSSDService::unregisterService() {
	boost::lock_guard<boost::mutex> lock(sdRefsMutex);

	assert(registerSDRef);
	interruptSelect();
	DNSServiceRefDeallocate(registerSDRef);
	registerSDRef = NULL;
}

void AppleDNSSDService::stop() {
	if (thread) {
		stopRequested = true;
		interruptSelect();
		thread->join();
		delete thread;
		stopRequested = false;
	}
}

void AppleDNSSDService::doStart() {
	// Listen for new services
	assert(!browseSDRef);
	DNSServiceErrorType result = DNSServiceBrowse(&browseSDRef, 0, 0, "_presence._tcp", 0, &AppleDNSSDService::handleServiceDiscoveredGlobal , this);
	if (result != kDNSServiceErr_NoError) {
		MainEventLoop::postEvent(boost::ref(onError), shared_from_this());
		return;
	}

	// Run the main loop
	while (!stopRequested) {
		fd_set fdSet;
		FD_ZERO(&fdSet);
		int maxSocket = 0;

		{
			boost::lock_guard<boost::mutex> lock(sdRefsMutex);

			// Browsing
			int browseSocket = DNSServiceRefSockFD(browseSDRef);
			maxSocket = browseSocket;
			FD_SET(browseSocket, &fdSet);

			// Registration
			if (registerSDRef) {
				int registerSocket = DNSServiceRefSockFD(registerSDRef);
				maxSocket = std::max(maxSocket, registerSocket);
				FD_SET(registerSocket, &fdSet);
			}
		}

		int selectResult = select(maxSocket+1, &fdSet, NULL, NULL, 0);

		{
			boost::lock_guard<boost::mutex> lock(sdRefsMutex);

			if (selectResult == -1) {
				MainEventLoop::postEvent(boost::ref(onError), shared_from_this());
				return;
			}
			if (selectResult == 0) {
				continue;
			}
			if (FD_ISSET(DNSServiceRefSockFD(browseSDRef), &fdSet)) {
				DNSServiceProcessResult(browseSDRef);
			}
			if (registerSDRef && FD_ISSET(DNSServiceRefSockFD(registerSDRef), &fdSet)) {
				DNSServiceProcessResult(registerSDRef);
			}
		}
	}

	if (registerSDRef) {
		DNSServiceRefDeallocate(registerSDRef);
		registerSDRef = NULL;
	}

	DNSServiceRefDeallocate(browseSDRef);
	browseSDRef = NULL;
}

void AppleDNSSDService::interruptSelect() {
	char c = 0;
	write(interruptSelectWriteSocket, &c, 1);
}

void AppleDNSSDService::handleServiceDiscoveredGlobal(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *serviceName, const char *regtype, const char *replyDomain, void *context ) {
	static_cast<AppleDNSSDService*>(context)->handleServiceDiscovered(sdRef, flags, interfaceIndex, errorCode, serviceName, regtype, replyDomain);
}

void AppleDNSSDService::handleServiceDiscovered(DNSServiceRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *serviceName, const char *regtype, const char *replyDomain) {
	if (errorCode != kDNSServiceErr_NoError) {
		MainEventLoop::postEvent(boost::ref(onError), shared_from_this());
	}
	else {
		Service service(serviceName, regtype, replyDomain, interfaceIndex);
		if (flags & kDNSServiceFlagsAdd) {
			MainEventLoop::postEvent(boost::bind(boost::ref(onServiceAdded), service), shared_from_this());
		}
		else {
			MainEventLoop::postEvent(boost::bind(boost::ref(onServiceRemoved), service), shared_from_this());
		}
	}
}

void AppleDNSSDService::handleServiceRegisteredGlobal(DNSServiceRef sdRef, DNSServiceFlags flags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context) {
	static_cast<AppleDNSSDService*>(context)->handleServiceRegistered(sdRef, flags, errorCode, name, regtype, domain);
}

void AppleDNSSDService::handleServiceRegistered(DNSServiceRef, DNSServiceFlags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain) {
	if (errorCode != kDNSServiceErr_NoError) {
		MainEventLoop::postEvent(boost::ref(onError), shared_from_this());
	}
	else {
		MainEventLoop::postEvent(boost::bind(boost::ref(onServiceRegistered), Service(name, regtype, domain, 0)), shared_from_this());
	}
}

}
