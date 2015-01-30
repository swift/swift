/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include "SystemMessageSnippet.h"

#include <QDateTime>

namespace Swift {

SystemMessageSnippet::SystemMessageSnippet(const QString& message, const QDateTime& time, bool appendToPrevious, QtChatTheme* theme, const QString& id, Direction direction) : ChatSnippet(appendToPrevious) {
	if (appendToPrevious) {
		setContinuationFallbackSnippet(boost::shared_ptr<ChatSnippet>(new SystemMessageSnippet(message, time, false, theme, id, direction)));
	}
	content_ = theme->getStatus();

	content_.replace("%direction%", directionToCSS(direction));
	content_.replace("%message%", wrapResizable("<span class='swift_message'>" + escape(message) + "</span>"));
	content_.replace("%shortTime%", wrapResizable(escape(time.toString("h:mm"))));
	content_.replace("%time%", wrapResizable("<span class='swift_time'>" + timeToEscapedString(time) + "</span>"));
	content_ = QString("<div id='%1'>%2</div>").arg(id).arg(content_);
}

SystemMessageSnippet::~SystemMessageSnippet() {

}

}
