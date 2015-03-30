/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/Tristate.h>
#include <Swiften/Elements/DiscoInfo.h>

namespace Swift {

class EntityCapsProvider;
class JID;
class PresenceOracle;

/**
 * @brief The FeatureOracle class enables direct feature support lookup for client features supported by Swiften.
 */
class SWIFTEN_API FeatureOracle {
	public:
		FeatureOracle(EntityCapsProvider* capsProvider, PresenceOracle* presenceOracle);

	public:
		Tristate isFileTransferSupported(const JID& jid);
		Tristate isMessageReceiptsSupported(const JID& jid);
		Tristate isMessageCorrectionSupported(const JID& jid);

	private:
		/**
		 * @brief getDiscoResultForJID returns a  shared reference to a DiscoInfo representing features supported by the jid.
		 * @param jid The JID to return the DiscoInfo::ref for.
		 * @return DiscoResult::ref
		 */
		DiscoInfo::ref getDiscoResultForJID(const JID& jid);

		Tristate isFeatureSupported(const JID& jid, const std::string& feature);

	private:
		EntityCapsProvider* capsProvider_;
		PresenceOracle* presenceOracle_;
};

}

