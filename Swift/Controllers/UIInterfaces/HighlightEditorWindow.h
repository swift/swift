/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <boost/signals2.hpp>

#include <Swift/Controllers/Contact.h>

namespace Swift {

class HighlightManager;

class HighlightEditorWindow {
public:
    HighlightEditorWindow();
    virtual ~HighlightEditorWindow();
    virtual void show() = 0;
    virtual void setHighlightManager(HighlightManager *highlightManager) = 0;
    virtual void setContactSuggestions(const std::vector<Contact::ref>& suggestions) = 0;

public:
    boost::signals2::signal<void (const std::string&)> onContactSuggestionsRequested;
};

}
