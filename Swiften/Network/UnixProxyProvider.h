/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/EnvironmentProxyProvider.h>

namespace Swift {
	class GConfProxyProvider;

	class UnixProxyProvider : public ProxyProvider {
		public:
			UnixProxyProvider();
			~UnixProxyProvider();

			virtual HostAddressPort getHTTPConnectProxy() const;
			virtual HostAddressPort getSOCKS5Proxy() const;

		private:
			GConfProxyProvider* gconfProxyProvider;
			EnvironmentProxyProvider environmentProxyProvider;
	};
}
