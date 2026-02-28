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

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/ProxyProvider.h>

namespace Swift {
    class SWIFTEN_API WindowsProxyProvider : public ProxyProvider {
        public:
            WindowsProxyProvider();
            virtual HostAddressPort getHTTPConnectProxy() const;
            virtual HostAddressPort getSOCKS5Proxy() const;
        private:
            HostAddressPort getAsHostAddressPort(std::string proxy);
            bool proxyEnabled(HKEY hKey) const;
            HostAddressPort socksProxy;
            HostAddressPort httpProxy;
    };
}
