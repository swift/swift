/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <vector>
#include <Swiften/Base/boost_bsignals.h>
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
    boost::signal<void (const std::string&)> onContactSuggestionsRequested;
};

}
