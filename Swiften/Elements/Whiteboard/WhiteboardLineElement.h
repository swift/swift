/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Whiteboard/WhiteboardColor.h>
#include <Swiften/Elements/Whiteboard/WhiteboardElement.h>

namespace Swift {
    class SWIFTEN_API WhiteboardLineElement : public WhiteboardElement {
    public:
        typedef std::shared_ptr<WhiteboardLineElement> ref;
    public:
        WhiteboardLineElement(int x1, int y1, int x2, int y2) : penWidth_(1) {
            x1_ = x1;
            y1_ = y1;
            x2_ = x2;
            y2_ = y2;
        }

        int x1() const {
            return x1_;
        }

        int y1() const {
            return y1_;
        }

        int x2() const {
            return x2_;
        }

        int y2() const {
            return y2_;
        }

        const WhiteboardColor& getColor() const {
            return color_;
        }

        void setColor(const WhiteboardColor& color) {
            color_ = color;
        }

        int getPenWidth() const {
            return penWidth_;
        }

        void setPenWidth(const int penWidth) {
            penWidth_ = penWidth;
        }

        void accept(WhiteboardElementVisitor& visitor) {
            visitor.visit(*this);
        }

    private:
        int x1_, y1_, x2_, y2_;
        WhiteboardColor color_;
        int penWidth_;
    };
}
