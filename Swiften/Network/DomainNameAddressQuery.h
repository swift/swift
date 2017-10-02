/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/optional.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Network/DomainNameResolveError.h>
#include <Swiften/Network/HostAddress.h>

namespace Swift {
    class DomainNameAddressQuery {
        public:
            typedef std::shared_ptr<DomainNameAddressQuery> ref;

            virtual ~DomainNameAddressQuery();

            virtual void run() = 0;

            boost::signals2::signal<void (const std::vector<HostAddress>&, boost::optional<DomainNameResolveError>)> onResult;
    };
}
