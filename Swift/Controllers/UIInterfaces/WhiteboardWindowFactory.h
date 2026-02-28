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

#include <memory>

namespace Swift {
    class WhiteboardSession;
    class WhiteboardWindow;

    class WhiteboardWindowFactory {
    public :
        virtual ~WhiteboardWindowFactory() {}

        virtual WhiteboardWindow* createWhiteboardWindow(std::shared_ptr<WhiteboardSession> whiteboardSession) = 0;
    };
}
