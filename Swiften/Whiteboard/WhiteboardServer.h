/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <list>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Whiteboard/WhiteboardInsertOperation.h>

namespace Swift {
    class SWIFTEN_API WhiteboardServer {
    public:
        void handleLocalOperationReceived(WhiteboardOperation::ref operation);
        WhiteboardOperation::ref handleClientOperationReceived(WhiteboardOperation::ref operation);
        void print();

    private:
        std::list<WhiteboardOperation::ref> operations_;
    };
}
