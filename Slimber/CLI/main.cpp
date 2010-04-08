/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <string>
#include <boost/bind.hpp>

#include "Swiften/Base/Platform.h"
#include "Slimber/Server.h"
#include "Slimber/FileVCardCollection.h"
#include "Swiften/LinkLocal/LinkLocalServiceBrowser.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDBrowseQuery.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDRegisterQuery.h"
//#include "Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuerier.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/Application/Platform/PlatformApplication.h"

using namespace Swift;

int main() {
	SimpleEventLoop eventLoop;
	/*
	boost::shared_ptr<BonjourQuerier> querier(new BonjourQuerier());
	querier->start();
	LinkLocalServiceBrowser browser(querier);
	browser.start();
	*/

/*
	FileVCardCollection vCardCollection(PlatformApplication("Slimber").getSettingsDir());
	Server server(5222, 5562, linkLocalRoster, dnsSDService, &vCardCollection);
	*/
	
	eventLoop.run();
	return 0;
}
