/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/DomainNameServiceQuery.h>

#include <cassert>
#include <functional>
#include <iterator>
#include <numeric>

#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Base/RandomGenerator.h>

using namespace Swift;

namespace {
    struct ResultPriorityComparator {
        bool operator()(const DomainNameServiceQuery::Result& a, const DomainNameServiceQuery::Result& b) const {
            return a.priority < b.priority;
        }
    };
}

namespace Swift {

DomainNameServiceQuery::~DomainNameServiceQuery() {
}

void DomainNameServiceQuery::sortResults(std::vector<DomainNameServiceQuery::Result>& queries, RandomGenerator& generator) {
    ResultPriorityComparator comparator;
    std::stable_sort(queries.begin(), queries.end(), comparator);

    std::vector<DomainNameServiceQuery::Result>::iterator i = queries.begin();
    while (i != queries.end()) {
        std::vector<DomainNameServiceQuery::Result>::iterator next = std::upper_bound(i, queries.end(), *i, comparator);
        if (std::distance(i, next) > 1) {
            std::vector<int> weights;
            std::transform(i, next, std::back_inserter(weights), [](const DomainNameServiceQuery::Result& result) {
                /* easy hack to account for '0' weights getting at least some weight */
                return result.weight + 1;
            });
            try {
                for (int j = 0; j < boost::numeric_cast<int>(weights.size()) - 1; ++j) {
                    std::vector<int> cumulativeWeights;
                    std::partial_sum(
                            weights.begin() + j,
                            weights.end(),
                            std::back_inserter(cumulativeWeights));
                    int randomNumber = generator.generateRandomInteger(cumulativeWeights.back());
                    auto selectedIndex = std::lower_bound(cumulativeWeights.begin(), cumulativeWeights.end(), randomNumber) - cumulativeWeights.begin();
                    std::swap(i[j], i[j + selectedIndex]);
                    std::swap(weights.begin()[j], weights.begin()[j + selectedIndex]);
                }
            }
            catch (const boost::numeric::bad_numeric_cast&) {
                // In the unlikely event of weights.size() being too large, use the list as-is.
            }
        }
        i = next;
    }
}


}
