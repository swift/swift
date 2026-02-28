/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

namespace Swift {
    class HighlightEditorWindow;

    class HighlightEditorWindowFactory {
    public :
        virtual ~HighlightEditorWindowFactory() {}

        virtual HighlightEditorWindow* createHighlightEditorWindow() = 0;
    };
}
