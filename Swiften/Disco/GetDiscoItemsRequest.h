/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/DiscoItems.h>
#include <Swiften/Queries/GenericRequest.h>

namespace Swift {
    class SWIFTEN_API GetDiscoItemsRequest : public GenericRequest<DiscoItems> {
        public:
            typedef boost::shared_ptr<GetDiscoItemsRequest> ref;

            static ref create(const JID& jid, IQRouter* router) {
                return ref(new GetDiscoItemsRequest(jid, router));
            }

            static ref create(const JID& jid, const std::string& node, IQRouter* router) {
                return ref(new GetDiscoItemsRequest(jid, node, router));
            }

        private:
            GetDiscoItemsRequest(const JID& jid, IQRouter* router) :
                    GenericRequest<DiscoItems>(IQ::Get, jid, boost::make_shared<DiscoItems>(), router) {
            }

            GetDiscoItemsRequest(const JID& jid, const std::string& node, IQRouter* router) :
                GenericRequest<DiscoItems>(IQ::Get, jid, boost::make_shared<DiscoItems>(), router) {
                getPayloadGeneric()->setNode(node);
            }
    };
}
