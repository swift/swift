/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Network/BoostNetworkFactories.h"
#include "Swiften/Network/BoostTimerFactory.h"
#include "Swiften/Network/BoostConnectionFactory.h"
#include <Swiften/Network/PlatformDomainNameResolver.h>

namespace Swift {

BoostNetworkFactories::BoostNetworkFactories(EventLoop* eventLoop) {
	timerFactory = new BoostTimerFactory(ioServiceThread.getIOService(), eventLoop);
	connectionFactory = new BoostConnectionFactory(ioServiceThread.getIOService(), eventLoop);
	domainNameResolver = new PlatformDomainNameResolver(eventLoop);
}

BoostNetworkFactories::~BoostNetworkFactories() {
	delete domainNameResolver;
	delete connectionFactory;
	delete timerFactory;
}

}
