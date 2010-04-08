/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Disco/CapsInfoGenerator.h"

#include <algorithm>

#include "Swiften/Base/foreach.h"
#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/StringCodecs/SHA1.h"
#include "Swiften/StringCodecs/Base64.h"

namespace Swift {

CapsInfoGenerator::CapsInfoGenerator(const String& node) : node_(node) {
}

CapsInfo CapsInfoGenerator::generateCapsInfo(const DiscoInfo& discoInfo) const {
	String serializedCaps;

	std::vector<DiscoInfo::Identity> identities(discoInfo.getIdentities());
	std::sort(identities.begin(), identities.end());
	foreach (const DiscoInfo::Identity& identity, identities) {
		serializedCaps += identity.getCategory() + "/" + identity.getType() + "/" + identity.getLanguage() + "/" + identity.getName() + "<";
	}

	std::vector<String> features(discoInfo.getFeatures());
	std::sort(features.begin(), features.end());
	foreach (const String& feature, features) {
		serializedCaps += feature + "<";
	}

	String version(Base64::encode(SHA1::getHash(serializedCaps)));
	return CapsInfo(node_, version, "sha-1");
}

}
