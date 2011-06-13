/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <Swiften/Base/foreach.h>
#include <Swiften/Network/UnixProxyProvider.h>
#if defined(HAVE_GCONF)
#  include "Swiften/Network/GConfProxyProvider.h"
#endif

namespace Swift {

UnixProxyProvider::UnixProxyProvider() :
	gconfProxyProvider(0),
	environmentProxyProvider()
{
#if defined(HAVE_GCONF)
	gconfProxyProvider = new GConfProxyProvider();
#endif
}

UnixProxyProvider::~UnixProxyProvider() {
	delete gconfProxyProvider;
}

HostAddressPort UnixProxyProvider::getSOCKS5Proxy() const {
	HostAddressPort proxy;
	if (gconfProxyProvider) {
		proxy = gconfProxyProvider.getSOCKS5Proxy();
		if(proxy.isValid()) {
			return proxy;
		}
	}
	proxy = environmentProxyProvider.getSOCKS5Proxy();
	if(proxy.isValid()) {
		return proxy;
	}
	return HostAddressPort(HostAddress(), 0);
}

HostAddressPort UnixProxyProvider::getHTTPConnectProxy() const {
	HostAddressPort proxy;
	if (gconfProxyProvider) {
		proxy = gconfProxyProvider.getHTTPConnectProxy();
		if(proxy.isValid()) {
			return proxy;
		}
	}
	proxy = environmentProxyProvider.getHTTPConnectProxy();
	if(proxy.isValid()) {
		return proxy;
	}
	return HostAddressPort(HostAddress(), 0);
}


}
