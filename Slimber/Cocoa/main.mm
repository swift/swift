#include <Cocoa/Cocoa.h>
#include <string>
#include <boost/bind.hpp>

#include "Swiften/LinkLocal/AppleDNSSDService.h"
#include "Swiften/EventLoop/Cocoa/CocoaEventLoop.h"
#include "Slimber/Server.h"

using namespace Swift;

int main(int argc, char *argv[]) {
	CocoaEventLoop eventLoop;

	boost::shared_ptr<DNSSDService> dnsSDService= 
		boost::shared_ptr<AppleDNSSDService>(new AppleDNSSDService());
	Server server(5222, 5562, dnsSDService);

	return NSApplicationMain(argc,  const_cast<const char **>(argv));
}
