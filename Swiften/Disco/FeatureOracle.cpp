/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Disco/FeatureOracle.h>

#include <algorithm>
#include <iterator>
#include <vector>

#include <Swiften/Base/foreach.h>
#include <Swiften/Disco/EntityCapsProvider.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/FileTransfer/FileTransferManager.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Presence/PresenceOracle.h>

namespace Swift {

FeatureOracle::FeatureOracle(EntityCapsProvider* capsProvider, PresenceOracle* presenceOracle) : capsProvider_(capsProvider), presenceOracle_(presenceOracle) {

}

Tristate FeatureOracle::isFileTransferSupported(const JID& jid) {
	DiscoInfo::ref discoInfo = getDiscoResultForJID(jid);
	if (discoInfo) {
		return FileTransferManager::isSupportedBy(discoInfo) ? Yes : No;
	}
	else {
		return Maybe;
	}
}

Tristate FeatureOracle::isMessageReceiptsSupported(const JID& jid) {
	return isFeatureSupported(jid, DiscoInfo::MessageDeliveryReceiptsFeature);
}

Tristate FeatureOracle::isMessageCorrectionSupported(const JID& jid) {
	return isFeatureSupported(jid, DiscoInfo::MessageCorrectionFeature);
}

DiscoInfo::ref FeatureOracle::getDiscoResultForJID(const JID& jid) {
	DiscoInfo::ref discoInfo;
	if (jid.isBare()) {
		// Calculate the common subset of disco features of all available results and return that.
		std::vector<Presence::ref> availablePresences =  presenceOracle_->getAllPresence(jid);

		bool commonFeaturesInitialized = false;
		std::vector<std::string> commonFeatures;
		foreach(Presence::ref presence, availablePresences) {
			DiscoInfo::ref presenceDiscoInfo = capsProvider_->getCaps(presence->getFrom());
			if (presenceDiscoInfo) {
				std::vector<std::string> features = presenceDiscoInfo->getFeatures();
				if (!commonFeaturesInitialized) {
					commonFeatures = features;
					commonFeaturesInitialized = true;
				}
				else {
					std::vector<std::string> featuresToRemove;
					foreach(const std::string& feature, commonFeatures) {
						if (std::find(features.begin(), features.end(), feature) == features.end()) {
							featuresToRemove.push_back(feature);
						}
					}
					foreach(const std::string& featureToRemove, featuresToRemove) {
						commonFeatures.erase(std::remove(commonFeatures.begin(), commonFeatures.end(), featureToRemove), commonFeatures.end());
					}
				}
			}
		}
		discoInfo = boost::make_shared<DiscoInfo>();

		foreach(const std::string& commonFeature, commonFeatures) {
			discoInfo->addFeature(commonFeature);
		}
	}
	else {
		// Return the disco result of the full JID.
		discoInfo = capsProvider_->getCaps(jid);
	}

	return discoInfo;
}

Tristate FeatureOracle::isFeatureSupported(const JID& jid, const std::string& feature) {
	DiscoInfo::ref discoInfo = getDiscoResultForJID(jid);
	if (discoInfo) {
		return discoInfo->hasFeature(feature) ? Yes : No;
	}
	else {
		return Maybe;
	}
}

}


