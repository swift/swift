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

#include <CoreFoundation/CoreFoundation.h>

#include <Swiften/Network/ProxyProvider.h>

namespace Swift {
    class MacOSXProxyProvider : public ProxyProvider {
        public:
            MacOSXProxyProvider();
            virtual HostAddressPort getHTTPConnectProxy() const;
            virtual HostAddressPort getSOCKS5Proxy() const;
    };
}
