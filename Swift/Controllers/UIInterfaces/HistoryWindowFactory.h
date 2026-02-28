/*
 * Copyright (c) 2012 Catalin Badea
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/HistoryWindow.h>

namespace Swift {
    class UIEventStream;
    class HistoryWindowFactory {
        public:
            virtual ~HistoryWindowFactory() {}
            virtual HistoryWindow* createHistoryWindow(UIEventStream* eventStream) = 0;
    };
}
