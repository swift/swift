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

#include <utility>

#include <Swiften/Elements/Whiteboard/WhiteboardDeleteOperation.h>
#include <Swiften/Elements/Whiteboard/WhiteboardInsertOperation.h>
#include <Swiften/Elements/Whiteboard/WhiteboardUpdateOperation.h>

namespace Swift {
    class WhiteboardTransformer {
    public:
        static std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> transform(WhiteboardOperation::ref clientOp, WhiteboardOperation::ref serverOp);
        static std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> transform(WhiteboardInsertOperation::ref clientOp, WhiteboardInsertOperation::ref serverOp);
        static std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> transform(WhiteboardUpdateOperation::ref clientOp, WhiteboardUpdateOperation::ref serverOp);
        static std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> transform(WhiteboardUpdateOperation::ref clientOp, WhiteboardInsertOperation::ref serverOp);
        static std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> transform(WhiteboardInsertOperation::ref clientOp, WhiteboardUpdateOperation::ref serverOp);
        static std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> transform(WhiteboardDeleteOperation::ref clientOp, WhiteboardDeleteOperation::ref serverOp);
        static std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> transform(WhiteboardInsertOperation::ref clientOp, WhiteboardDeleteOperation::ref serverOp);
        static std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> transform(WhiteboardDeleteOperation::ref clientOp, WhiteboardInsertOperation::ref serverOp);
        static std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> transform(WhiteboardUpdateOperation::ref clientOp, WhiteboardDeleteOperation::ref serverOp);
        static std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> transform(WhiteboardDeleteOperation::ref clientOp, WhiteboardUpdateOperation::ref serverOp);
    };
}
