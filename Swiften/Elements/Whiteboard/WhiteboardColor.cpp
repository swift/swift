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

#include <Swiften/Elements/Whiteboard/WhiteboardColor.h>

#include <cstdio>
#include <iomanip>
#include <sstream>

#include <Swiften/Base/String.h>

namespace Swift {
    WhiteboardColor::WhiteboardColor() : red_(0), green_(0), blue_(0), alpha_(255) {
    }

    WhiteboardColor::WhiteboardColor(int red, int green, int blue, int alpha) : red_(red), green_(green), blue_(blue), alpha_(alpha) {
    }

    WhiteboardColor::WhiteboardColor(const std::string& hex) : alpha_(255) {
        int value = String::convertHexStringToInt(hex.substr(1));
        red_ = (value >> 16)&0xFF;
        green_ = (value >> 8)&0xFF;
        blue_ = value&0xFF;
    }

    std::string WhiteboardColor::toHex() const {
        std::string value = String::convertIntToHexString((red_ << 16) + (green_ << 8) + blue_);
        while (value.size() < 6) {
            value.insert(0, "0");
        }
        return "#"+value;
    }

    int WhiteboardColor::getRed() const {
        return red_;
    }

    int WhiteboardColor::getGreen() const {
        return green_;
    }

    int WhiteboardColor::getBlue() const {
        return blue_;
    }

    int WhiteboardColor::getAlpha() const {
        return alpha_;
    }

    void WhiteboardColor::setAlpha(int alpha) {
        alpha_ = alpha;
    }
}
