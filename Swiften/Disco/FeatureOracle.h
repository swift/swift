/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <unordered_map>
#include <unordered_set>

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
        Tristate isWhiteboardSupported(const JID& jid);

        JID getMostAvailableClientForFileTrasfer(const JID& bareJID);

    private:
        /**
         * @brief getDiscoResultForJID returns a  shared reference to a DiscoInfo representing features supported by the jid.
         * @param jid The JID to return an std::unordered_map<std::string, Tristate> for.
         * @return std::unordered_map<std::string, Tristate>
         */
        std::unordered_map<std::string, Tristate> getFeaturesForJID(const JID& jid);

        Tristate isFeatureSupported(const std::unordered_map<std::string, Tristate>& supportedFeatures, const std::string& feature);

    private:
        EntityCapsProvider* capsProvider_;
        PresenceOracle* presenceOracle_;
};

}

