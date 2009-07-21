#include "Slimber/Cocoa/Slimber.h"

#include "Swiften/Base/foreach.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/LinkLocal/AppleDNSSDService.h"
#include "Swiften/Application/Platform/PlatformApplication.h"
#include "Slimber/Cocoa/Menulet.h"
#include "Slimber/Server.h"
#include "Slimber/FileVCardCollection.h"

using namespace Swift;

Slimber::Slimber() {
	dnsSDService = boost::shared_ptr<AppleDNSSDService>(new AppleDNSSDService());

	linkLocalRoster = boost::shared_ptr<LinkLocalRoster>(new LinkLocalRoster(dnsSDService));
	linkLocalRoster->onRosterChanged.connect(boost::bind(&Slimber::handleRosterChanged, this));

	vCardCollection = new FileVCardCollection(PlatformApplication("Slimber").getSettingsDir());

	server = new Server(5222, 5562, linkLocalRoster, dnsSDService, vCardCollection);
	server->onSelfConnected.connect(boost::bind(&Slimber::handleSelfConnected, this, _1));

	menulet = [[Menulet alloc] init];
	handleRosterChanged();
}

Slimber::~Slimber() {
	[menulet release];
	delete server;
	delete vCardCollection;
}

void Slimber::handleSelfConnected(bool b) {
	[menulet setSelfConnected: b];
}

void Slimber::handleRosterChanged() {
	NSMutableArray* names = [[NSMutableArray alloc] init];
	boost::shared_ptr<RosterPayload> roster = linkLocalRoster->getRoster();
	foreach(const RosterItemPayload& item, roster->getItems()) {
		NSString* name = [NSString stringWithUTF8String: item.getName().getUTF8Data()];
		[names addObject: name];
		[name release];
	}

	[menulet setUserNames: names];
	[names release];
}
