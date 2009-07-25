#include "Swiften/LinkLocal/AppleDNSSDService.h"

#include <algorithm>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"
#include "Swiften/Network/HostAddress.h"

namespace Swift {

AppleDNSSDService::AppleDNSSDService() : thread(0), stopRequested(false), haveError(false), browseSDRef(0), registerSDRef(0) {
	int fds[2];
	int result = pipe(fds);
	assert(result == 0);
	interruptSelectReadSocket = fds[0];
	fcntl(interruptSelectReadSocket, F_SETFL, fcntl(interruptSelectReadSocket, F_GETFL)|O_NONBLOCK);
	interruptSelectWriteSocket = fds[1];
}

AppleDNSSDService::~AppleDNSSDService() {
	stop();
}

void AppleDNSSDService::start() {
	stop();
	thread = new boost::thread(boost::bind(&AppleDNSSDService::doStart, shared_from_this()));
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

void AppleDNSSDService::registerService(const String& name, int port, const LinkLocalServiceInfo& info) {
	boost::lock_guard<boost::mutex> lock(sdRefsMutex);

	assert(!registerSDRef);
	ByteArray txtRecord = info.toTXTRecord();
	DNSServiceErrorType result = DNSServiceRegister(&registerSDRef, 0, 0, name.getUTF8Data(), "_presence._tcp", NULL, NULL, port, txtRecord.getSize(), txtRecord.getData(), &AppleDNSSDService::handleServiceRegisteredGlobal, this);
	if (result != kDNSServiceErr_NoError) {
		std::cerr << "Error creating service registration" << std::endl;
		haveError = true;
	}

	interruptSelect();
}

void AppleDNSSDService::updateService(const LinkLocalServiceInfo& info) {
	boost::lock_guard<boost::mutex> lock(sdRefsMutex);
	assert(registerSDRef);
	ByteArray txtRecord = info.toTXTRecord();
	DNSServiceUpdateRecord(registerSDRef, NULL, NULL, txtRecord.getSize(), txtRecord.getData(), 0);
}

void AppleDNSSDService::unregisterService() {
	boost::lock_guard<boost::mutex> lock(sdRefsMutex);

	assert(registerSDRef);
	DNSServiceRefDeallocate(registerSDRef); // Interrupts select()
	registerSDRef = NULL;
}

void AppleDNSSDService::startResolvingService(const DNSSDServiceID& service) {
	boost::lock_guard<boost::mutex> lock(sdRefsMutex);

	DNSServiceRef resolveSDRef;
	DNSServiceErrorType result = DNSServiceResolve(&resolveSDRef, 0, service.getNetworkInterfaceID(), service.getName().getUTF8Data(), service.getType().getUTF8Data(), service.getDomain().getUTF8Data(), &AppleDNSSDService::handleServiceResolvedGlobal, this);
	if (result != kDNSServiceErr_NoError) {
		std::cerr << "Error creating service resolve query" << std::endl;
		haveError = true;
	}
	else {
		bool isNew = resolveSDRefs.insert(std::make_pair(service, resolveSDRef)).second;
		assert(isNew);
	}

	interruptSelect();
}

void AppleDNSSDService::stopResolvingService(const DNSSDServiceID& service) {
	boost::lock_guard<boost::mutex> lock(sdRefsMutex);

	ServiceSDRefMap::iterator i = resolveSDRefs.find(service);
	assert(i != resolveSDRefs.end());
	DNSServiceRefDeallocate(i->second); // Interrupts select()
	resolveSDRefs.erase(i);
}

void AppleDNSSDService::resolveHostname(const String& hostname, int interfaceIndex) {
	boost::lock_guard<boost::mutex> lock(sdRefsMutex);

	DNSServiceRef hostnameResolveSDRef;
	DNSServiceErrorType result = DNSServiceGetAddrInfo(&hostnameResolveSDRef, 0, interfaceIndex, kDNSServiceProtocol_IPv4, hostname.getUTF8Data(), &AppleDNSSDService::handleHostnameResolvedGlobal, this);
	if (result != kDNSServiceErr_NoError) {
		std::cerr << "Error creating hostname resolve query" << std::endl;
		haveError = true;
	}
	else {
		hostnameResolveSDRefs.push_back(hostnameResolveSDRef);
	}

	interruptSelect();
}

void AppleDNSSDService::doStart() {
	haveError = false;
	onStarted();

	// Listen for new services
	{
		boost::lock_guard<boost::mutex> lock(sdRefsMutex);
		assert(!browseSDRef);
		DNSServiceErrorType result = DNSServiceBrowse(&browseSDRef, 0, 0, "_presence._tcp", 0, &AppleDNSSDService::handleServiceDiscoveredGlobal , this);
		if (result != kDNSServiceErr_NoError) {
			std::cerr << "Error creating browse query" << std::endl;
			haveError = true;
		}
	}

	// Run the main loop
	while (!haveError && !stopRequested) {
		fd_set fdSet;
		FD_ZERO(&fdSet);
		int maxSocket = interruptSelectReadSocket;
		FD_SET(interruptSelectReadSocket, &fdSet);

		{
			boost::lock_guard<boost::mutex> lock(sdRefsMutex);

			// Browsing
			int browseSocket = DNSServiceRefSockFD(browseSDRef);
			maxSocket = std::max(maxSocket, browseSocket);
			FD_SET(browseSocket, &fdSet);

			// Registration
			if (registerSDRef) {
				int registerSocket = DNSServiceRefSockFD(registerSDRef);
				maxSocket = std::max(maxSocket, registerSocket);
				FD_SET(registerSocket, &fdSet);
			}

			// Service resolving
			for (ServiceSDRefMap::const_iterator i = resolveSDRefs.begin(); i != resolveSDRefs.end(); ++i) {
				int resolveSocket = DNSServiceRefSockFD(i->second);
				maxSocket = std::max(maxSocket, resolveSocket);
				FD_SET(resolveSocket, &fdSet);
			}

			// Hostname resolving
			for (HostnameSDRefs::const_iterator i = hostnameResolveSDRefs.begin(); i != hostnameResolveSDRefs.end(); ++i) {
				int hostnameResolveSocket = DNSServiceRefSockFD(*i);
				maxSocket = std::max(maxSocket, hostnameResolveSocket);
				FD_SET(hostnameResolveSocket, &fdSet);
			}
		}

		int selectResult = select(maxSocket+1, &fdSet, NULL, NULL, 0);

		// Flush the interruptSelectReadSocket
		if (FD_ISSET(interruptSelectReadSocket, &fdSet)) {
			char dummy;
			while (read(interruptSelectReadSocket, &dummy, 1) > 0) {}
		}

		{
			boost::lock_guard<boost::mutex> lock(sdRefsMutex);
			if (selectResult <= 0) {
				continue;
			}
			if (FD_ISSET(DNSServiceRefSockFD(browseSDRef), &fdSet)) {
				DNSServiceProcessResult(browseSDRef);
			}
			if (registerSDRef && FD_ISSET(DNSServiceRefSockFD(registerSDRef), &fdSet)) {
				DNSServiceProcessResult(registerSDRef);
			}
			for (ServiceSDRefMap::const_iterator i = resolveSDRefs.begin(); i != resolveSDRefs.end(); ++i) {
				if (FD_ISSET(DNSServiceRefSockFD(i->second), &fdSet)) {
					DNSServiceProcessResult(i->second);
				}
			}
			for (HostnameSDRefs::const_iterator i = hostnameResolveSDRefs.begin(); i != hostnameResolveSDRefs.end(); ++i) {
				if (FD_ISSET(DNSServiceRefSockFD(*i), &fdSet)) {
					DNSServiceProcessResult(*i);
					hostnameResolveSDRefs.erase(std::remove(hostnameResolveSDRefs.begin(), hostnameResolveSDRefs.end(), *i), hostnameResolveSDRefs.end());
					DNSServiceRefDeallocate(*i);
					break; // Stop the loop, because we removed an element
				}
			}
		}
	}

	{
		boost::lock_guard<boost::mutex> lock(sdRefsMutex);

		for (ServiceSDRefMap::const_iterator i = resolveSDRefs.begin(); i != resolveSDRefs.end(); ++i) {
			DNSServiceRefDeallocate(i->second);
		}
		resolveSDRefs.clear();

		for (HostnameSDRefs::const_iterator i = hostnameResolveSDRefs.begin(); i != hostnameResolveSDRefs.end(); ++i) {
			DNSServiceRefDeallocate(*i);
		}
		hostnameResolveSDRefs.clear();

		if (registerSDRef) {
			DNSServiceRefDeallocate(registerSDRef);
			registerSDRef = NULL;
		}

		DNSServiceRefDeallocate(browseSDRef);
		browseSDRef = NULL;
	}

	MainEventLoop::postEvent(boost::bind(boost::ref(onStopped), haveError), shared_from_this());
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
		return;
	}
	else {
		DNSSDServiceID service(serviceName, regtype, replyDomain, interfaceIndex);
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
		std::cerr << "Error registering service" << std::endl;
		haveError = true;
	}
	else {
		MainEventLoop::postEvent(boost::bind(boost::ref(onServiceRegistered), DNSSDServiceID(name, regtype, domain, 0)), shared_from_this());
	}
}

void AppleDNSSDService::handleServiceResolvedGlobal(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const unsigned char *txtRecord, void *context ) {
	static_cast<AppleDNSSDService*>(context)->handleServiceResolved(sdRef, flags, interfaceIndex, errorCode, fullname, hosttarget, port, txtLen, txtRecord);
}

void AppleDNSSDService::handleServiceResolved(DNSServiceRef sdRef, DNSServiceFlags, uint32_t, DNSServiceErrorType errorCode, const char *, const char *hosttarget, uint16_t port, uint16_t txtLen, const unsigned char *txtRecord) {
	if (errorCode != kDNSServiceErr_NoError) {
		return;
	}
	for (ServiceSDRefMap::const_iterator i = resolveSDRefs.begin(); i != resolveSDRefs.end(); ++i) {
		if (i->second == sdRef) {
			MainEventLoop::postEvent(boost::bind(boost::ref(onServiceResolved), i->first, ResolveResult(hosttarget, port, LinkLocalServiceInfo::createFromTXTRecord(ByteArray(reinterpret_cast<const char*>(txtRecord), txtLen)))), shared_from_this());
			return;
		}
	}
	assert(false);
}

void AppleDNSSDService::handleHostnameResolvedGlobal(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *hostname, const struct sockaddr *address, uint32_t ttl, void *context) {
	static_cast<AppleDNSSDService*>(context)->handleHostnameResolved(sdRef, flags, interfaceIndex, errorCode, hostname, address, ttl);
} 

void AppleDNSSDService::handleHostnameResolved(DNSServiceRef, DNSServiceFlags, uint32_t, DNSServiceErrorType errorCode, const char *hostname, const struct sockaddr *rawAddress, uint32_t) {
	if (errorCode) {
		std::cerr << "Error resolving hostname" << std::endl;
		MainEventLoop::postEvent(boost::bind(boost::ref(onHostnameResolved), hostname, boost::optional<HostAddress>()), shared_from_this());
	}
	else {
		assert(rawAddress->sa_family == AF_INET);
		const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(rawAddress);
		uint32_t address = ntohl(sa->sin_addr.s_addr);
		MainEventLoop::postEvent(boost::bind(boost::ref(onHostnameResolved), String(hostname), HostAddress(reinterpret_cast<unsigned char*>(&address), 4)), shared_from_this());
	}
}

}
