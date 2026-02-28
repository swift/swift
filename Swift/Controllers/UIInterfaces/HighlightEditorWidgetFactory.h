/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

namespace Swift {

    class HighlightEditorWidget;

    class HighlightEditorWidgetFactory {
        public:
            virtual ~HighlightEditorWidgetFactory() {}

            virtual HighlightEditorWidget* createHighlightEditorWidget() = 0;
    };

}
