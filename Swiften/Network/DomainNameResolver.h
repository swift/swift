/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swiften/Base/API.h>

namespace Swift {
    class DomainNameServiceQuery;
    class DomainNameAddressQuery;


    class SWIFTEN_API DomainNameResolver {
        public:
            virtual ~DomainNameResolver();

            virtual std::shared_ptr<DomainNameServiceQuery> createServiceQuery(const std::string& serviceLookupPrefix, const std::string& domain) = 0;
            virtual std::shared_ptr<DomainNameAddressQuery> createAddressQuery(const std::string& name) = 0;
    };
}
