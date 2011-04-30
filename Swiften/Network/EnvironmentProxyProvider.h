/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/ProxyProvider.h>

namespace Swift {
	class EnvironmentProxyProvider : public ProxyProvider {
		public:
			EnvironmentProxyProvider();
			virtual HostAddressPort getHTTPConnectProxy() const;
			virtual HostAddressPort getSOCKS5Proxy() const;
		private:
			HostAddressPort getFromEnv(const char* envVarName, std::string proxyProtocol);
			HostAddressPort socksProxy;
			HostAddressPort httpProxy;
	};
}


