#include <dns_sd.h>
#include <iostream>
#include <sys/socket.h>

#include "Swiften/Network/HostAddress.h"

using namespace Swift;

void handleServiceRegistered(DNSServiceRef sdRef, DNSServiceFlags flags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context ) {
	std::cerr << "Service registered " << name << " " << regtype << " " << domain << std::endl;
}

void handleServiceDiscovered(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *serviceName, const char *regtype, const char *replyDomain, void *context ) {
	std::cerr << "Service discovered " << interfaceIndex << " " << serviceName << " " << regtype << " " << replyDomain << " " << flags << std::endl;
}

void handleServiceResolved( DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const unsigned char *txtRecord, void *context ) {
	std::cerr << "Service resolved " << fullname << " " << hosttarget << " " << port << " " << txtLen << " " << /*std::string((const char*) txtRecord, txtLen) <<*/ std::endl;
}

void handleAddressInfoReceived ( DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *hostname, const struct sockaddr *address, uint32_t ttl, void *context ) {
	std::cerr << "Address received " << HostAddress((const unsigned char*) address->sa_data, 4).toString() << std::endl;
} 

int main(int argc, char* argv[]) {
	fd_set fdSet;
	DNSServiceErrorType result;

	DNSServiceRef registerSDRef;
	result = DNSServiceRegister(&registerSDRef, 0, 0, "eemi", "_presence._tcp", NULL, NULL, 5269, 0, NULL, handleServiceRegistered, NULL);
	if (result != kDNSServiceErr_NoError) {
		std::cerr << "Error 1" << std::endl;
	}
	int registerSocket = DNSServiceRefSockFD(registerSDRef);
	FD_ZERO(&fdSet);
	FD_SET(registerSocket, &fdSet);
	select(registerSocket+1, &fdSet, &fdSet, &fdSet, 0);
	DNSServiceProcessResult(registerSDRef);

	DNSServiceRef browseSDRef;
	result = DNSServiceBrowse(&browseSDRef, 0, 0, "_presence._tcp", 0, handleServiceDiscovered , 0);
	if (result != kDNSServiceErr_NoError) {
		std::cerr << "Error 2" << std::endl;
	}
	int browseSocket = DNSServiceRefSockFD(browseSDRef);
	//while(true) {
		FD_ZERO(&fdSet);
		FD_SET(browseSocket, &fdSet);
		select(browseSocket+1, &fdSet, &fdSet, &fdSet, 0);
		DNSServiceProcessResult(browseSDRef);
	//}


	DNSServiceRef resolveSDRef;
	result = DNSServiceResolve(&resolveSDRef, 0, 6, "Remko@Micro", "_presence._tcp.", "local.", handleServiceResolved , 0);
	if (result != kDNSServiceErr_NoError) {
		std::cerr << "Error 3" << std::endl;
	}
	int resolveSocket = DNSServiceRefSockFD(resolveSDRef);
	//while(true) {
		FD_ZERO(&fdSet);
		FD_SET(resolveSocket, &fdSet);
		select(resolveSocket+1, &fdSet, &fdSet, &fdSet, 0);
		DNSServiceProcessResult(resolveSDRef);
	//}


	DNSServiceRef addressSDRef;
	result = DNSServiceGetAddrInfo(&addressSDRef, 0, 6, kDNSServiceProtocol_IPv4, "Micro.local.", handleAddressInfoReceived, 0);
	if (result != kDNSServiceErr_NoError) {
		std::cerr << "Error 4" << std::endl;
	}
	int addressSocket = DNSServiceRefSockFD(addressSDRef);
	//while(true) {
	std::cout << "GetAddrInfo2" << std::endl;
		FD_ZERO(&fdSet);
		FD_SET(addressSocket, &fdSet);
		select(addressSocket+1, &fdSet, &fdSet, &fdSet, 0);
		DNSServiceProcessResult(addressSDRef);
	//}

	// DNSServiceRefDeallocate
	
	return 0;
}
