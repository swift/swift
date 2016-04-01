/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Queries/GenericRequest.h>

namespace Swift {
    class SWIFTEN_API GetDiscoInfoRequest : public GenericRequest<DiscoInfo> {
        public:
            typedef std::shared_ptr<GetDiscoInfoRequest> ref;

            static ref create(const JID& jid, IQRouter* router) {
                return ref(new GetDiscoInfoRequest(jid, router));
            }

            static ref create(const JID& jid, const std::string& node, IQRouter* router) {
                return ref(new GetDiscoInfoRequest(jid, node, router));
            }

        private:
            GetDiscoInfoRequest(const JID& jid, IQRouter* router) :
                    GenericRequest<DiscoInfo>(IQ::Get, jid, std::make_shared<DiscoInfo>(), router) {
            }

            GetDiscoInfoRequest(const JID& jid, const std::string& node, IQRouter* router) :
                    GenericRequest<DiscoInfo>(IQ::Get, jid, std::make_shared<DiscoInfo>(), router) {
                getPayloadGeneric()->setNode(node);
            }
    };
}
