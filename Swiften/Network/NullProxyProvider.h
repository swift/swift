/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/ProxyProvider.h>

namespace Swift {
	class NullProxyProvider : public ProxyProvider {
		public:
			NullProxyProvider();

			virtual HostAddressPort getHTTPConnectProxy() const;
			virtual HostAddressPort getSOCKS5Proxy() const;
	};
}
