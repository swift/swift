/*
 * Copyright (c) 2011-2016 Isode Limited.
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
    class NATPMPInterface : public NATTraversalInterface, boost::noncopyable {
        public:
            NATPMPInterface();
            virtual ~NATPMPInterface();

            virtual bool isAvailable();

            virtual boost::optional<HostAddress> getPublicIP();
            virtual boost::optional<NATPortMapping> addPortForward(int localPort, int publicPort);
            virtual bool removePortForward(const NATPortMapping&);

        private:
            struct Private;
            const std::unique_ptr<Private> p;
    };
}
