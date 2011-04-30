/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <Swiften/Base/Log.h>
#include <Swiften/Network/EnvironmentProxyProvider.h>

namespace Swift {

EnvironmentProxyProvider::EnvironmentProxyProvider() {
	socksProxy = getFromEnv("all_proxy", "socks");
	httpProxy = getFromEnv("http_proxy", "http");
	SWIFT_LOG(debug) << "Environment: SOCKS5 => " << socksProxy.toString() << "; HTTP Connect => " << httpProxy.toString() << std::endl;
}

HostAddressPort EnvironmentProxyProvider::getHTTPConnectProxy() const {
	return httpProxy;
}

HostAddressPort EnvironmentProxyProvider::getSOCKS5Proxy() const {
	return socksProxy;
}

HostAddressPort EnvironmentProxyProvider::getFromEnv(const char* envVarName, std::string proxyProtocol) {
	char* envVar = NULL;
	std::string address;
	int port = 0;

	envVar = getenv(envVarName);

	proxyProtocol += "://";
	address = envVar != NULL ? envVar : "0.0.0.0";
	if(envVar != NULL && address.compare(0, proxyProtocol.length(), proxyProtocol) == 0) {
		address = address.substr(proxyProtocol.length(), address.length());
		port = atoi(address.substr(address.find(':') + 1, address.length()).c_str());
		address = address.substr(0, address.find(':'));
	}

	return HostAddressPort(HostAddress(address), port);
}

}
