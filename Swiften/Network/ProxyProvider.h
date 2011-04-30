/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once
#include <map>

#include <Swiften/Base/Log.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Base/String.h>

namespace Swift {
	class ProxyProvider {
		public:
			ProxyProvider();
			virtual ~ProxyProvider();
			virtual HostAddressPort getHTTPConnectProxy() const = 0;
			virtual HostAddressPort getSOCKS5Proxy() const = 0;
	};
}

