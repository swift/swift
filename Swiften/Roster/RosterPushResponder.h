/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/RosterPayload.h>
#include <Swiften/Queries/SetResponder.h>

namespace Swift {
    class SWIFTEN_API RosterPushResponder : public SetResponder<RosterPayload> {
        public:
            RosterPushResponder(IQRouter* router) : SetResponder<RosterPayload>(router) {}

        public:
            boost::signals2::signal<void (std::shared_ptr<RosterPayload>)> onRosterReceived;

        private:
            virtual bool handleSetRequest(const JID& from, const JID&, const std::string& id, std::shared_ptr<RosterPayload> payload) {
                if (getIQRouter()->isAccountJID(from)) {
                    onRosterReceived(payload);
                    sendResponse(from, id, std::shared_ptr<RosterPayload>());
                }
                else {
                    sendError(from, id, ErrorPayload::NotAuthorized, ErrorPayload::Cancel);
                }
                return true;
            }
    };
}
