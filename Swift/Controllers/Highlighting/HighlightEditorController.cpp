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

#include <Swift/Controllers/Highlighting/HighlightEditorController.h>

#include <boost/bind.hpp>

#include <Swift/Controllers/ContactSuggester.h>
#include <Swift/Controllers/UIEvents/RequestHighlightEditorUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIInterfaces/HighlightEditorWindow.h>
#include <Swift/Controllers/UIInterfaces/HighlightEditorWindowFactory.h>

namespace Swift {

HighlightEditorController::HighlightEditorController(UIEventStream* uiEventStream, HighlightEditorWindowFactory* highlightEditorWindowFactory, HighlightManager* highlightManager)
: highlightEditorWindowFactory_(highlightEditorWindowFactory), highlightEditorWindow_(nullptr), highlightManager_(highlightManager), contactSuggester_(nullptr)
{
    uiEventStream->onUIEvent.connect(boost::bind(&HighlightEditorController::handleUIEvent, this, _1));
}

HighlightEditorController::~HighlightEditorController()
{
    delete highlightEditorWindow_;
    highlightEditorWindow_ = nullptr;
}

void HighlightEditorController::handleUIEvent(std::shared_ptr<UIEvent> rawEvent)
{
    std::shared_ptr<RequestHighlightEditorUIEvent> event = std::dynamic_pointer_cast<RequestHighlightEditorUIEvent>(rawEvent);
    if (event) {
        if (!highlightEditorWindow_) {
            highlightEditorWindow_ = highlightEditorWindowFactory_->createHighlightEditorWindow();
            highlightEditorWindow_->setHighlightManager(highlightManager_);
            highlightEditorWindow_->onContactSuggestionsRequested.connect(boost::bind(&HighlightEditorController::handleContactSuggestionsRequested, this, _1));
        }
        highlightEditorWindow_->show();
    }
}

void HighlightEditorController::handleContactSuggestionsRequested(const std::string& text)
{
    if (contactSuggester_) {
        highlightEditorWindow_->setContactSuggestions(contactSuggester_->getSuggestions(text, true));
    }
}

}
