/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>

namespace Swift {
    class SWIFTEN_API WhiteboardColor {
    public:
        WhiteboardColor();
        WhiteboardColor(int red, int green, int blue, int alpha = 255);
        WhiteboardColor(const std::string& hex);
        std::string toHex() const;
        int getRed() const;
        int getGreen() const;
        int getBlue() const;
        int getAlpha() const;
        void setAlpha(int alpha);

    private:
        int red_, green_, blue_;
        int alpha_;
    };
}
