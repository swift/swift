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
#include <Swiften/Elements/Whiteboard/WhiteboardElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardOperation.h>

namespace Swift {
    class SWIFTEN_API WhiteboardInsertOperation : public WhiteboardOperation {
    public:
        typedef std::shared_ptr<WhiteboardInsertOperation> ref;
    public:
        WhiteboardElement::ref getElement() const {
            return element_;
        }

        void setElement(WhiteboardElement::ref element) {
            element_ = element;
        }

    private:
        WhiteboardElement::ref element_;
    };
}
