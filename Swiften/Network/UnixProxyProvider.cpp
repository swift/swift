/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/UnixProxyProvider.h>

#if defined(HAVE_GCONF)
#  include "Swiften/Network/GConfProxyProvider.h"
#endif

namespace Swift {

UnixProxyProvider::UnixProxyProvider() :
    gconfProxyProvider(nullptr),
    environmentProxyProvider()
{
#if defined(HAVE_GCONF)
    gconfProxyProvider = new GConfProxyProvider();
#endif
}

UnixProxyProvider::~UnixProxyProvider() {
#if defined(HAVE_GCONF)
    delete gconfProxyProvider;
#else
    (void)gconfProxyProvider;
#endif
}

HostAddressPort UnixProxyProvider::getSOCKS5Proxy() const {
    HostAddressPort proxy;
#if defined(HAVE_GCONF)
    proxy = gconfProxyProvider->getSOCKS5Proxy();
    if(proxy.isValid()) {
        return proxy;
    }
#endif
    proxy = environmentProxyProvider.getSOCKS5Proxy();
    if(proxy.isValid()) {
        return proxy;
    }
    return HostAddressPort(HostAddress(), 0);
}

HostAddressPort UnixProxyProvider::getHTTPConnectProxy() const {
    HostAddressPort proxy;
#if defined(HAVE_GCONF)
    proxy = gconfProxyProvider->getHTTPConnectProxy();
    if(proxy.isValid()) {
        return proxy;
    }
#endif
    proxy = environmentProxyProvider.getHTTPConnectProxy();
    if(proxy.isValid()) {
        return proxy;
    }
    return HostAddressPort(HostAddress(), 0);
}


}
