/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/DomainNameServiceQuery.h>

#include <numeric>
#include <cassert>

#include <Swiften/Base/RandomGenerator.h>
#include <boost/numeric/conversion/cast.hpp>

using namespace Swift;

namespace {
	struct ResultPriorityComparator {
		bool operator()(const DomainNameServiceQuery::Result& a, const DomainNameServiceQuery::Result& b) const {
			return a.priority < b.priority;
		}
	};

	struct WeightAccumulator {
			int operator()(int accumulator, const DomainNameServiceQuery::Result& result) {
				return accumulator + result.weight;
			}
	};

	struct WeightToProbability {
			WeightToProbability(int total) : total(total) {}

			double operator()(const DomainNameServiceQuery::Result& result) {
				return result.weight / boost::numeric_cast<double>(total);
			}
			int total;
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
			int weightSum = std::accumulate(i, next, 0, WeightAccumulator());
			std::vector<double> probabilities;
			std::transform(i, next, std::back_inserter(probabilities), WeightToProbability(weightSum));

			// Shuffling the result array and the probabilities in parallel
			for (size_t j = 0; j < probabilities.size(); ++j) {
				int selectedIndex = generator.generateWeighedRandomNumber(probabilities.begin() + j, probabilities.end());
				std::swap(i[j], i[j + selectedIndex]);
				std::swap(probabilities.begin()[j], probabilities.begin()[j + selectedIndex]);
			}
		}
		i = next;
	}
}


}
