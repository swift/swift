/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Network/NullProxyProvider.h>

using namespace Swift;

NullProxyProvider::NullProxyProvider() {
}

HostAddressPort NullProxyProvider::getHTTPConnectProxy() const {
	return HostAddressPort();
}

HostAddressPort NullProxyProvider::getSOCKS5Proxy() const {
	return HostAddressPort();
}
