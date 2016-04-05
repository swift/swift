/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/RosterPayload.h>
#include <Swiften/Queries/Request.h>

namespace Swift {
    class SWIFTEN_API SetRosterRequest : public Request {
        public:
            typedef std::shared_ptr<SetRosterRequest> ref;

            static ref create(RosterPayload::ref payload, IQRouter* router) {
                return ref(new SetRosterRequest(JID(), payload, router));
            }

            static ref create(RosterPayload::ref payload, const JID& to, IQRouter* router) {
                return ref(new SetRosterRequest(to, payload, router));
            }

        private:
            SetRosterRequest(const JID& to, std::shared_ptr<RosterPayload> payload, IQRouter* router) : Request(IQ::Set, to, std::shared_ptr<RosterPayload>(payload), router) {
            }

            virtual void handleResponse(std::shared_ptr<Payload> /*payload*/, ErrorPayload::ref error) {
                onResponse(error);
            }

        public:
            boost::signals2::signal<void (ErrorPayload::ref)> onResponse;
    };
}
