/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {

    class UIEventStream;

    class HighlightEditorWindowFactory;
    class HighlightEditorWindow;

    class HighlightManager;
    class ContactSuggester;

    class HighlightEditorController {
        public:
            HighlightEditorController(UIEventStream* uiEventStream, HighlightEditorWindowFactory* highlightEditorWindowFactory, HighlightManager* highlightManager);
            ~HighlightEditorController();

            HighlightManager* getHighlightManager() const { return highlightManager_; }
            void setContactSuggester(ContactSuggester *suggester) { contactSuggester_ = suggester; }

        private:
            void handleUIEvent(std::shared_ptr<UIEvent> event);
            void handleContactSuggestionsRequested(const std::string& text);

        private:
            HighlightEditorWindowFactory* highlightEditorWindowFactory_;
            HighlightEditorWindow* highlightEditorWindow_;
            HighlightManager* highlightManager_;
            ContactSuggester* contactSuggester_;
    };

}
