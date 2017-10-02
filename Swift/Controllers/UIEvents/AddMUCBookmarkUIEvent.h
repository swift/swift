/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/MUC/MUCBookmark.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
    class AddMUCBookmarkUIEvent : public UIEvent {
        public:
            AddMUCBookmarkUIEvent(const MUCBookmark& bookmark) : bookmark(bookmark) {}
            const MUCBookmark& getBookmark() { return bookmark; }

        private:
            MUCBookmark bookmark;
    };
}
