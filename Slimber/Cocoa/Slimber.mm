#include "Slimber/Cocoa/Slimber.h"

#include <boost/bind.hpp>

#include "Swiften/Base/foreach.h"
#include "Swiften/Application/Platform/PlatformApplication.h"
#include "Swiften/LinkLocal/LinkLocalService.h"
#include "Swiften/LinkLocal/LinkLocalServiceBrowser.h"
#include "Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuerier.h"
#include "Slimber/Cocoa/Menulet.h"
#include "Slimber/Server.h"
#include "Slimber/FileVCardCollection.h"

using namespace Swift;

Slimber::Slimber() {
	dnsSDQuerier = boost::shared_ptr<BonjourQuerier>(new BonjourQuerier());
	dnsSDQuerier->start();

	linkLocalServiceBrowser = new LinkLocalServiceBrowser(dnsSDQuerier);
	linkLocalServiceBrowser->onServiceAdded.connect(
			boost::bind(&Slimber::handleServicesChanged, this));
	linkLocalServiceBrowser->onServiceRemoved.connect(
			boost::bind(&Slimber::handleServicesChanged, this));
	linkLocalServiceBrowser->onServiceChanged.connect(
			boost::bind(&Slimber::handleServicesChanged, this));
	linkLocalServiceBrowser->start();

	vCardCollection = new FileVCardCollection(
			PlatformApplication("Slimber").getSettingsDir());

	server = new Server(5222, 5562, linkLocalServiceBrowser, vCardCollection);
	server->onStopped.connect(
			boost::bind(&Slimber::handleServerStopped, this, _1));
	server->onSelfConnected.connect(
			boost::bind(&Slimber::handleSelfConnected, this, _1));

	menulet = [[Menulet alloc] init];
	handleSelfConnected(false);
	handleServicesChanged();

	server->start();
}

Slimber::~Slimber() {
	[menulet release];
	delete server;
	delete vCardCollection;
	linkLocalServiceBrowser->stop();
	delete linkLocalServiceBrowser;
	dnsSDQuerier->stop();
}

void Slimber::handleSelfConnected(bool b) {
	if (b) {
		[menulet setXMPPStatus: @"You are logged in" online: true];
	}
	else {
		[menulet setXMPPStatus: @"You are not logged in" online: false];
	}
}

void Slimber::handleServicesChanged() {
	NSMutableArray* names = [[NSMutableArray alloc] init];
	foreach(const LinkLocalService& service, linkLocalServiceBrowser->getServices()) {
		[names addObject: [NSString stringWithUTF8String: service.getDescription().getUTF8Data()]];
	}

	[menulet setUserNames: names];
	[names release];
}

void Slimber::handleServerStopped(boost::optional<ServerError> error) {
	if (error) {
		[menulet setXMPPStatus: @"XMPP Server Error." online: false];
	}
	else {
		[menulet setXMPPStatus: @"XMPP Server Not Runnning." online: false];
	}
}
