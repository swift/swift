/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once
#include <Swiften/Network/ProxyProvider.h>
#include <CoreFoundation/CoreFoundation.h>

namespace Swift {
	class MacOSXProxyProvider : public ProxyProvider {
		public:
			MacOSXProxyProvider();
			virtual HostAddressPort getHTTPConnectProxy() const;
			virtual HostAddressPort getSOCKS5Proxy() const;
	};
}
