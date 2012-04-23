/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/BoostRandomGenerator.h>

#include <numeric>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

namespace Swift {

int BoostRandomGenerator::generateWeighedRandomNumber(std::vector<double>::const_iterator probabilities_begin, std::vector<double>::const_iterator probabilities_end) {
	// Only works starting boost 1.47
	//boost::random::discrete_distribution<> distribution(weights.begin(), weights.end());
	//return distribution(generator);

	std::vector<double> cumulative;
	std::partial_sum(probabilities_begin, probabilities_end, std::back_inserter(cumulative));
	boost::uniform_real<> dist(0, cumulative.back());
	boost::variate_generator<boost::mt19937&, boost::uniform_real<> > die(generator, dist);
	return std::lower_bound(cumulative.begin(), cumulative.end(), die()) - cumulative.begin();
}

}
