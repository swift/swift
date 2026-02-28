/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Network/DomainNameResolveError.h>

namespace Swift {
    class RandomGenerator;

    class SWIFTEN_API DomainNameServiceQuery {
        public:
            typedef std::shared_ptr<DomainNameServiceQuery> ref;

            struct Result {
                Result(const std::string& hostname = "", unsigned short port = 0, int priority = -1, int weight = -1) : hostname(hostname), port(port), priority(priority), weight(weight) {}
                std::string hostname;
                unsigned short port;
                int priority;
                int weight;
            };

            virtual ~DomainNameServiceQuery();

            virtual void run() = 0;
            static void sortResults(std::vector<DomainNameServiceQuery::Result>& queries, RandomGenerator& generator);

            boost::signals2::signal<void (const std::vector<Result>&)> onResult;
    };
}
