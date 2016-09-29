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

#include <string>

#include <boost/smart_ptr/shared_ptr.hpp>

#include <Swiften/Base/API.h>

namespace Swift {
    class WhiteboardOperation {
    public:
        typedef std::shared_ptr<WhiteboardOperation> ref;
    public:
        WhiteboardOperation() {}
        SWIFTEN_DEFAULT_COPY_CONSTRUCTOR(WhiteboardOperation)
        virtual ~WhiteboardOperation(){}

        std::string getID() const {
            return id_;
        }

        void setID(const std::string& id) {
            id_ = id;
        }

        std::string getParentID() const {
            return parentID_;
        }

        void setParentID(const std::string& parentID) {
            parentID_ = parentID;
        }

        int getPos() const {
            return pos_;
        }

        void setPos(int pos) {
            pos_ = pos;
        }

    private:
        std::string id_;
        std::string parentID_;
        int pos_ = 0;
    };
}
