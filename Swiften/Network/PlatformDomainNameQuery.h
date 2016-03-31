/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

namespace Swift {
    class PlatformDomainNameResolver;

    class PlatformDomainNameQuery {
        public:
            typedef boost::shared_ptr<PlatformDomainNameQuery> ref;

            PlatformDomainNameQuery(PlatformDomainNameResolver* resolver) : resolver(resolver) {}
            virtual ~PlatformDomainNameQuery() {}

            virtual void runBlocking() = 0;

        protected:
            PlatformDomainNameResolver* getResolver() {
                return resolver;
            }

        private:
            PlatformDomainNameResolver* resolver;
    };
}
