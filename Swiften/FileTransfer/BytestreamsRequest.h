/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Bytestreams.h>
#include <Swiften/Queries/GenericRequest.h>

namespace Swift {
    class SWIFTEN_API BytestreamsRequest : public GenericRequest<Bytestreams> {
        public:
            typedef std::shared_ptr<BytestreamsRequest> ref;

            static ref create(const JID& jid, std::shared_ptr<Bytestreams> payload, IQRouter* router) {
                return ref(new BytestreamsRequest(jid, payload, router));
            }

            static ref create(const JID& from, const JID& to, std::shared_ptr<Bytestreams> payload, IQRouter* router) {
                return ref(new BytestreamsRequest(from, to, payload, router));
            }

        private:
            BytestreamsRequest(const JID& jid, std::shared_ptr<Bytestreams> payload, IQRouter* router) : GenericRequest<Bytestreams>(IQ::Set, jid, payload, router) {
            }

            BytestreamsRequest(const JID& from, const JID& to, std::shared_ptr<Bytestreams> payload, IQRouter* router) : GenericRequest<Bytestreams>(IQ::Set, from, to, payload, router) {
            }
    };
}
