/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/WhiteboardPayload.h>
#include <Swiften/Queries/SetResponder.h>

namespace Swift {
    class IQRouter;
    class WhiteboardSessionManager;

    class SWIFTEN_API WhiteboardResponder : public SetResponder<WhiteboardPayload> {
    public:
        WhiteboardResponder(WhiteboardSessionManager* sessionManager, IQRouter* router);
        bool handleSetRequest(const JID& from, const JID& /*to*/, const std::string& id, std::shared_ptr<WhiteboardPayload> payload);

    private:
        WhiteboardSessionManager* sessionManager_;
        IQRouter* router_;
    };
}
