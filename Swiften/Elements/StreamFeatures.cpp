/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Elements/StreamFeatures.h>

#include <algorithm>

namespace Swift {

bool StreamFeatures::hasCompressionMethod(const std::string& mechanism) const {
	return std::find(compressionMethods_.begin(), compressionMethods_.end(), mechanism) != compressionMethods_.end();
}

bool StreamFeatures::hasAuthenticationMechanism(const std::string& mechanism) const {
	return std::find(authenticationMechanisms_.begin(), authenticationMechanisms_.end(), mechanism) != authenticationMechanisms_.end();
}

}
