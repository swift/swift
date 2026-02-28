/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/SystemMessageSnippet.h>

#include <QDateTime>

namespace Swift {

SystemMessageSnippet::SystemMessageSnippet(const QString& message, const QDateTime& time, bool appendToPrevious, QtChatTheme* theme, const QString& id, Direction direction) : ChatSnippet(appendToPrevious) {
    if (appendToPrevious) {
        setContinuationFallbackSnippet(std::make_shared<SystemMessageSnippet>(message, time, false, theme, id, direction));
    }
    content_ = theme->getStatus();

    content_.replace("%direction%", directionToCSS(direction));
    content_.replace("%message%", wrapResizable("<span class='swift_message'>" + escape(message) + "</span>"));
    content_.replace("%shortTime%", wrapResizable(escape(time.toString("h:mm"))));
    content_.replace("%time%", wrapResizable("<span class='swift_time'>" + timeToEscapedString(time) + "</span>"));
    content_.replace("%id%", id);
}

SystemMessageSnippet::~SystemMessageSnippet() {

}

}
