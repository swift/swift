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
    class SWIFTEN_API WhiteboardDeleteOperation : public WhiteboardOperation {
    public:
        typedef std::shared_ptr<WhiteboardDeleteOperation> ref;
    public:
        std::string getElementID() const {
            return elementID_;
        }

        void setElementID(const std::string& elementID) {
            elementID_ = elementID;
        }

    private:
        std::string elementID_;
    };
}
