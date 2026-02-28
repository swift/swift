/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>

#include <Swiften/Base/API.h>
#include <Swiften/Base/String.h>
#include <Swiften/Network/HostAddressPort.h>

namespace Swift {
    class SWIFTEN_API ProxyProvider {
        public:
            ProxyProvider();
            virtual ~ProxyProvider();
            virtual HostAddressPort getHTTPConnectProxy() const = 0;
            virtual HostAddressPort getSOCKS5Proxy() const = 0;
    };
}

