/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/JinglePayload.h>
#include <Swiften/Queries/SetResponder.h>

namespace Swift {
    class IQRouter;
    class JingleSessionManager;

    class SWIFTEN_API JingleResponder : public SetResponder<JinglePayload> {
        public:
            JingleResponder(JingleSessionManager* sessionManager, IQRouter* router);
            virtual ~JingleResponder();
        private:
            virtual bool handleSetRequest(const JID& from, const JID& to, const std::string& id, std::shared_ptr<JinglePayload> payload);

        private:
            JingleSessionManager* sessionManager;
            IQRouter* router;
    };
}
