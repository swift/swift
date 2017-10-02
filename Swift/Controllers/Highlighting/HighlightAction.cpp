/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Highlighting/HighlightAction.h>

namespace Swift {



bool operator ==(const HighlightAction& a, const HighlightAction& b) {
    if (a.getFrontColor() != b.getFrontColor()) {
        return false;
    }
    if (a.getBackColor() != b.getBackColor()) {
        return false;
    }
    if (a.getSoundFilePath() != b.getSoundFilePath()) {
        return false;
    }
    if (a.isSystemNotificationEnabled() != b.isSystemNotificationEnabled()) {
        return false;
    }
    return true;
}

bool operator !=(const HighlightAction& a, const HighlightAction& b) {
    return !(a == b);
}

void HighlightAction::setFrontColor(const boost::optional<std::string>& frontColor) {
    frontColor_ = frontColor;
}

boost::optional<std::string> HighlightAction::getFrontColor() const {
    return frontColor_;
}

void HighlightAction::setBackColor(const boost::optional<std::string>& backColor) {
    backColor_ = backColor;
}

boost::optional<std::string> HighlightAction::getBackColor() const {
    return backColor_;
}

void HighlightAction::setSoundFilePath(const boost::optional<std::string>& soundFilePath) {
    soundFilePath_ = soundFilePath;
}

boost::optional<std::string> HighlightAction::getSoundFilePath() const {
    return soundFilePath_;
}

void HighlightAction::setSystemNotificationEnabled(bool systemNotificationEnabled) {
    systemNotificaitonEnabled_ = systemNotificationEnabled;
}

bool HighlightAction::isSystemNotificationEnabled() const {
    return systemNotificaitonEnabled_;
}

bool HighlightAction::isEmpty() const {
    return !frontColor_.is_initialized() && !backColor_.is_initialized() && !soundFilePath_.is_initialized() && !systemNotificaitonEnabled_;
}

}
