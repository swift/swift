/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StreamInitiation.h>
#include <Swiften/Queries/GenericRequest.h>

namespace Swift {
    class SWIFTEN_API StreamInitiationRequest : public GenericRequest<StreamInitiation> {
        public:
            typedef std::shared_ptr<StreamInitiationRequest> ref;

            static ref create(const JID& jid, std::shared_ptr<StreamInitiation> payload, IQRouter* router) {
                return ref(new StreamInitiationRequest(jid, payload, router));
            }

            static ref create(const JID& from, const JID& to, std::shared_ptr<StreamInitiation> payload, IQRouter* router) {
                return ref(new StreamInitiationRequest(from, to, payload, router));
            }

        private:
            StreamInitiationRequest(const JID& jid, std::shared_ptr<StreamInitiation> payload, IQRouter* router) : GenericRequest<StreamInitiation>(IQ::Set, jid, payload, router) {
            }

            StreamInitiationRequest(const JID& from, const JID& to, std::shared_ptr<StreamInitiation> payload, IQRouter* router) : GenericRequest<StreamInitiation>(IQ::Set, from, to, payload, router) {
            }
    };
}
