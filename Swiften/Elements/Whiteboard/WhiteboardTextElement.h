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
#include <Swiften/Elements/Whiteboard/WhiteboardColor.h>
#include <Swiften/Elements/Whiteboard/WhiteboardElement.h>

namespace Swift {
    class SWIFTEN_API WhiteboardTextElement : public WhiteboardElement {
    public:
        typedef std::shared_ptr<WhiteboardTextElement> ref;
    public:
        WhiteboardTextElement(int x, int y) {
            x_ = x;
            y_ = y;
        }

        void setText(const std::string text) {
            text_ = text;
        }

        const std::string& getText() const {
            return text_;
        }

        int getX() const {
            return x_;
        }

        int getY() const {
            return y_;
        }

        const WhiteboardColor& getColor() const {
            return color_;
        }

        void setColor(const WhiteboardColor& color) {
            color_ = color;
        }

        int getSize() const {
            return size_;
        }

        void setSize(const int size) {
            size_ = size;
        }

        void accept(WhiteboardElementVisitor& visitor) {
            visitor.visit(*this);
        }

    private:
        int x_, y_;
        int size_ = 0;
        std::string text_;
        WhiteboardColor color_;
    };
}
