/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/BoostNetworkFactories.h>
#include <Swiften/Network/BoostTimerFactory.h>
#include <Swiften/Network/BoostConnectionFactory.h>
#include <Swiften/Network/PlatformDomainNameResolver.h>
#include <Swiften/Network/BoostConnectionServerFactory.h>
#include <Swiften/Network/PlatformNATTraversalWorker.h>
#include <Swiften/Parser/PlatformXMLParserFactory.h>
#include <Swiften/Network/NullNATTraverser.h>
#include <Swiften/TLS/PlatformTLSFactories.h>
#include <Swiften/Network/PlatformProxyProvider.h>

namespace Swift {

BoostNetworkFactories::BoostNetworkFactories(EventLoop* eventLoop) : eventLoop(eventLoop){
	timerFactory = new BoostTimerFactory(ioServiceThread.getIOService(), eventLoop);
	connectionFactory = new BoostConnectionFactory(ioServiceThread.getIOService(), eventLoop);
	domainNameResolver = new PlatformDomainNameResolver(eventLoop);
	connectionServerFactory = new BoostConnectionServerFactory(ioServiceThread.getIOService(), eventLoop);
#ifdef SWIFT_EXPERIMENTAL_FT
	natTraverser = new PlatformNATTraversalWorker(eventLoop);
#else
	natTraverser = new NullNATTraverser(eventLoop);
#endif
	xmlParserFactory = new PlatformXMLParserFactory();
	tlsFactories = new PlatformTLSFactories();
	proxyProvider = new PlatformProxyProvider();
}

BoostNetworkFactories::~BoostNetworkFactories() {
	delete proxyProvider;
	delete tlsFactories;
	delete xmlParserFactory;
	delete natTraverser;
	delete connectionServerFactory;
	delete domainNameResolver;
	delete connectionFactory;
	delete timerFactory;
}

TLSContextFactory* BoostNetworkFactories::getTLSContextFactory() const {
	return tlsFactories->getTLSContextFactory();
}

}
