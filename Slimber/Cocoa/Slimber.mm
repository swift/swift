#include "Slimber/Cocoa/Slimber.h"

#include "Swiften/LinkLocal/AppleDNSSDService.h"
#include "Slimber/Cocoa/Menulet.h"
#include "Slimber/Server.h"

using namespace Swift;

Slimber::Slimber() {
	dnsSDService = boost::shared_ptr<AppleDNSSDService>(new AppleDNSSDService());
	server = new Server(5222, 5562, dnsSDService);
	menulet = [[Menulet alloc] init];
}

Slimber::~Slimber() {
	delete server;
	[menulet release];
}
