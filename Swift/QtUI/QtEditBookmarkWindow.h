/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/UIEvents/EditMUCBookmarkUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>

#include <Swift/QtUI/QtBookmarkDetailWindow.h>

namespace Swift {
    class QtEditBookmarkWindow : public QtBookmarkDetailWindow {
        Q_OBJECT
        public:
            QtEditBookmarkWindow(UIEventStream* eventStream, const MUCBookmark& bookmark);
            bool commit();

        private:
            UIEventStream* eventStream_;
            MUCBookmark bookmark_;
    };
}
