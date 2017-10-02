/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Queries/GetResponder.h>

namespace Swift {
    class IQRouter;

    class SWIFTEN_API JIDDiscoInfoResponder : public GetResponder<DiscoInfo> {
        public:
            JIDDiscoInfoResponder(IQRouter* router);

            void clearDiscoInfo(const JID& jid);
            void setDiscoInfo(const JID& jid, const DiscoInfo& info);
            void setDiscoInfo(const JID& jid, const std::string& node, const DiscoInfo& info);

        private:
            virtual bool handleGetRequest(const JID& from, const JID& to, const std::string& id, std::shared_ptr<DiscoInfo> payload);

        private:
            struct JIDDiscoInfo {
                DiscoInfo discoInfo;
                std::map<std::string, DiscoInfo> nodeDiscoInfo;
            };
            typedef std::map<JID, JIDDiscoInfo> JIDDiscoInfoMap;
            JIDDiscoInfoMap info;
    };
}
