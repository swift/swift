/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

namespace Swift {

    class HighlightManager;

    class HighlightEditorWidget {
        public:
            virtual ~HighlightEditorWidget() {}

            virtual void show() = 0;

            virtual void setHighlightManager(HighlightManager* highlightManager) = 0;
    };

}
