/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once
#if defined(HAVE_GCONF)
#  include "Swiften/Network/GConfProxyProvider.h"
#endif
#include <Swiften/Network/EnvironmentProxyProvider.h>

namespace Swift {
	class UnixProxyProvider : public ProxyProvider {
		public:
			UnixProxyProvider();
			virtual HostAddressPort getHTTPConnectProxy() const;
			virtual HostAddressPort getSOCKS5Proxy() const;
		private:
#if defined(HAVE_GCONF)
			GConfProxyProvider gconfProxyProvider;
#endif
			EnvironmentProxyProvider environmentProxyProvider;
	};
}
