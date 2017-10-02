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

#include <boost/signals2.hpp>

namespace Swift {
    class WhiteboardSession;
    class WhiteboardElement;

    class WhiteboardWindow {
    public:
        virtual ~WhiteboardWindow() {}

        virtual void show() = 0;
        virtual void setSession(std::shared_ptr<WhiteboardSession> session) = 0;
        virtual void activateWindow() = 0;
        virtual void setName(const std::string& name) = 0;
    };
}
