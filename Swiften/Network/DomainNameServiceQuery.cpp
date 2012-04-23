/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/DomainNameServiceQuery.h>

#include <numeric>
#include <cassert>
#include <functional>
#include <iterator>

#include <Swiften/Base/RandomGenerator.h>
#include <boost/numeric/conversion/cast.hpp>

using namespace Swift;

namespace {
	struct ResultPriorityComparator {
		bool operator()(const DomainNameServiceQuery::Result& a, const DomainNameServiceQuery::Result& b) const {
			return a.priority < b.priority;
		}
	};

	struct GetWeight {
			GetWeight() {}

			int operator()(const DomainNameServiceQuery::Result& result) {
				return result.weight + 1 /* easy hack to account for '0' weights getting at least some weight */;
			}
	};
}

namespace Swift {

DomainNameServiceQuery::~DomainNameServiceQuery() {
}

void DomainNameServiceQuery::sortResults(std::vector<DomainNameServiceQuery::Result>& queries, RandomGenerator& generator) {
	ResultPriorityComparator comparator;
	std::sort(queries.begin(), queries.end(), comparator);

	std::vector<DomainNameServiceQuery::Result>::iterator i = queries.begin();
	while (i != queries.end()) {
		std::vector<DomainNameServiceQuery::Result>::iterator next = std::upper_bound(i, queries.end(), *i, comparator);
		if (std::distance(i, next) > 1) {
			std::vector<int> weights;
			std::transform(i, next, std::back_inserter(weights), GetWeight());
			for (size_t j = 0; j < weights.size() - 1; ++j) {
				std::vector<int> cumulativeWeights;
				std::partial_sum(weights.begin() + j, weights.end(), std::back_inserter(cumulativeWeights));
				int randomNumber = generator.generateRandomInteger(cumulativeWeights.back());
				int selectedIndex = std::lower_bound(cumulativeWeights.begin(), cumulativeWeights.end(), randomNumber) - cumulativeWeights.begin();
				std::swap(i[j], i[j + selectedIndex]);
				std::swap(weights.begin()[j], weights.begin()[j + selectedIndex]);
			}
		}
		i = next;
	}
}


}
