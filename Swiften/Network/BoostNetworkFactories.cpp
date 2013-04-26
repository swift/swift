/*
 * Copyright (c) 2010-2013 Remko Tronçon
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
#include <Swiften/Network/PlatformNetworkEnvironment.h>
#include <Swiften/TLS/PlatformTLSFactories.h>
#include <Swiften/Network/PlatformProxyProvider.h>
#include <Swiften/IDN/PlatformIDNConverter.h>
#include <Swiften/IDN/IDNConverter.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/Crypto/CryptoProvider.h>

namespace Swift {

BoostNetworkFactories::BoostNetworkFactories(EventLoop* eventLoop) : eventLoop(eventLoop){
	timerFactory = new BoostTimerFactory(ioServiceThread.getIOService(), eventLoop);
	connectionFactory = new BoostConnectionFactory(ioServiceThread.getIOService(), eventLoop);
	connectionServerFactory = new BoostConnectionServerFactory(ioServiceThread.getIOService(), eventLoop);
#ifdef SWIFT_EXPERIMENTAL_FT
	natTraverser = new PlatformNATTraversalWorker(eventLoop);
#else
	natTraverser = new NullNATTraverser(eventLoop);
#endif
	networkEnvironment = new PlatformNetworkEnvironment();
	xmlParserFactory = new PlatformXMLParserFactory();
	tlsFactories = new PlatformTLSFactories();
	proxyProvider = new PlatformProxyProvider();
	idnConverter = PlatformIDNConverter::create();
	domainNameResolver = new PlatformDomainNameResolver(idnConverter, eventLoop);
	cryptoProvider = PlatformCryptoProvider::create();
}

BoostNetworkFactories::~BoostNetworkFactories() {
	delete cryptoProvider;
	delete domainNameResolver;
	delete idnConverter;
	delete proxyProvider;
	delete tlsFactories;
	delete xmlParserFactory;
	delete networkEnvironment;
	delete natTraverser;
	delete connectionServerFactory;
	delete connectionFactory;
	delete timerFactory;
}

TLSContextFactory* BoostNetworkFactories::getTLSContextFactory() const {
	return tlsFactories->getTLSContextFactory();
}

}
