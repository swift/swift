/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Highlighting/HighlightConfiguration.h>

namespace Swift {

bool operator ==(const HighlightConfiguration& a, const HighlightConfiguration& b) {
    if (a.keywordHighlights != b.keywordHighlights) {
        return false;
    }
    if (a.contactHighlights != b.contactHighlights) {
        return false;
    }
    if (a.ownMentionAction != b.ownMentionAction) {
        return false;
    }
    if (a.playSoundOnIncomingDirectMessages != b.playSoundOnIncomingDirectMessages) {
        return false;
    }
    if (a.showNotificationOnIncomingDirectMessages != b.showNotificationOnIncomingDirectMessages) {
        return false;
    }
    if (a.playSoundOnIncomingGroupchatMessages != b.playSoundOnIncomingGroupchatMessages) {
        return false;
    }
    if (a.showNotificationOnIncomingGroupchatMessages != b.showNotificationOnIncomingGroupchatMessages) {
        return false;
    }
    return true;
}

}
