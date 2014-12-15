/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include "SystemMessageSnippet.h"

#include <QDateTime>

namespace Swift {

SystemMessageSnippet::SystemMessageSnippet(const QString& message, const QDateTime& time, bool appendToPrevious, QtChatTheme* theme, Direction direction) : ChatSnippet(appendToPrevious) {
	if (appendToPrevious) {
		setContinuationFallbackSnippet(boost::shared_ptr<ChatSnippet>(new SystemMessageSnippet(message, time, false, theme, direction)));
	}
	content_ = theme->getStatus();

	content_.replace("%direction%", directionToCSS(direction));
	content_.replace("%message%", wrapResizable("<span class='swift_message'>" + escape(message) + "</span>"));
	content_.replace("%shortTime%", wrapResizable(escape(time.toString("h:mm"))));
	content_.replace("%time%", wrapResizable("<span class='swift_time'>" + timeToEscapedString(time) + "</span>"));
}

SystemMessageSnippet::~SystemMessageSnippet() {

}

}
