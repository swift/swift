/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/ProxyProvider.h>

namespace Swift {
	class GConfProxyProvider : public ProxyProvider {
		public:
			GConfProxyProvider();
			virtual HostAddressPort getHTTPConnectProxy() const;
			virtual HostAddressPort getSOCKS5Proxy() const;
		private:
			HostAddressPort getFromGConf(const char* gcHost, const char* gcPort);
			HostAddressPort socksProxy;
			HostAddressPort httpProxy;
	};
}


