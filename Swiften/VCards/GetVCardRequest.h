/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/Queries/GenericRequest.h>

namespace Swift {
    class SWIFTEN_API GetVCardRequest : public GenericRequest<VCard> {
        public:
            typedef std::shared_ptr<GetVCardRequest> ref;

            static ref create(const JID& jid, IQRouter* router) {
                return ref(new GetVCardRequest(jid, router));
            }

        private:
            GetVCardRequest(const JID& jid, IQRouter* router) : GenericRequest<VCard>(IQ::Get, jid, std::make_shared<VCard>(), router) {
            }
    };
}
