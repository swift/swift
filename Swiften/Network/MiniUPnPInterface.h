/*
 * Copyright (c) 2011-2018 Isode Limited.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>

#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>

#include <Swiften/Network/NATPortMapping.h>
#include <Swiften/Network/NATTraversalInterface.h>

namespace Swift {
    class MiniUPnPInterface : public NATTraversalInterface, boost::noncopyable {
        public:
            MiniUPnPInterface();
            virtual ~MiniUPnPInterface();

            virtual bool isAvailable();

            boost::optional<HostAddress> getPublicIP();
            boost::optional<NATPortMapping> addPortForward(unsigned short localPort, unsigned short publicPort);
            bool removePortForward(const NATPortMapping&);

        private:
            struct Private;
            const std::unique_ptr<Private> p;
    };
}
