/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/signals2.hpp>

#include <Swiften/LinkLocal/DNSSD/DNSSDServiceID.h>

namespace Swift {
    class DNSSDBrowseQuery {
        public:
            virtual ~DNSSDBrowseQuery();

            virtual void startBrowsing() = 0;
            virtual void stopBrowsing() = 0;

            boost::signals2::signal<void (const DNSSDServiceID&)> onServiceAdded;
            boost::signals2::signal<void (const DNSSDServiceID&)> onServiceRemoved;
            boost::signals2::signal<void ()> onError;
    };
}
