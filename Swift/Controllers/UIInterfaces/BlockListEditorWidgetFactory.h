/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

namespace Swift {

    class BlockListEditorWidget;

    class BlockListEditorWidgetFactory {
        public:
            virtual ~BlockListEditorWidgetFactory() {}

            virtual BlockListEditorWidget* createBlockListEditorWidget() = 0;
    };

}
