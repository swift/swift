/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/MessageSnippet.h>

#include <QDateTime>

namespace Swift {

MessageSnippet::MessageSnippet(const QString& message, const QString& sender, const QDateTime& time, const QString& iconURI, bool isIncoming, bool appendToPrevious, QtChatTheme* theme, const QString& id, Direction direction) : ChatSnippet(appendToPrevious) {
    if (appendToPrevious) {
        setContinuationFallbackSnippet(std::make_shared<MessageSnippet>(message, sender, time, iconURI, isIncoming, false, theme, id, direction));
    }
    if (isIncoming) {
        if (appendToPrevious) {
            content_ = theme->getIncomingNextContent();
        }
        else {
            content_ = theme->getIncomingContent();
        }
    }
    else {
        if (appendToPrevious) {
            content_ = theme->getOutgoingNextContent();
        }
        else {
            content_ = theme->getOutgoingContent();
        }
    }

    content_.replace("%direction%", directionToCSS(direction));
    content_.replace("%message%", wrapResizable("<span class='swift_message'>" + escape(message) + "</span>"));
    content_.replace("%wrapped_sender%", wrapResizable(escape(sender)));
    content_.replace("%sender%", escape(sender));
    content_.replace("%time%", wrapResizable("<span class='swift_time'>" + timeToEscapedString(time) + "</span>"));
    content_.replace("%userIconPath%", escape(iconURI));
    content_.replace("%id%", id);
}

MessageSnippet::~MessageSnippet() {

}

}
