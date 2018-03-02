/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/RosterPayload.h>
#include <Swiften/Queries/GenericRequest.h>

namespace Swift {
    class SWIFTEN_API GetRosterRequest : public GenericRequest<RosterPayload> {
        public:
            typedef std::shared_ptr<GetRosterRequest> ref;

            static ref create(IQRouter* router, bool hasAnnotate = false) {
                return ref(new GetRosterRequest(router, hasAnnotate));
            }

            static ref create(IQRouter* router, const std::string& version, bool hasAnnotate = false) {
                ref result(new GetRosterRequest(router, hasAnnotate));
                result->getPayloadGeneric()->setVersion(version);
                return result;
            }

        private:
            GetRosterRequest(IQRouter* router, bool hasAnnotate) :
                    GenericRequest<RosterPayload>(IQ::Get, JID(), std::make_shared<RosterPayload>(hasAnnotate), router) {
            }
    };
}
