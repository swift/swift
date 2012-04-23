/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "SystemMessageSnippet.h"

#include <QDateTime>

namespace Swift {

SystemMessageSnippet::SystemMessageSnippet(const QString& message, const QDateTime& time, bool appendToPrevious, QtChatTheme* theme) : ChatSnippet(appendToPrevious) {
	if (appendToPrevious) {
		setContinuationFallbackSnippet(boost::shared_ptr<ChatSnippet>(new SystemMessageSnippet(message, time, false, theme)));
	}
	content_ = theme->getStatus();

	content_.replace("%message%", wrapResizable("<span class='swift_message'>" + escape(message) + "</span>"));
	content_.replace("%shortTime%", wrapResizable(escape(time.toString("h:mm"))));
	content_.replace("%time%", wrapResizable("<span class='swift_time'>" + timeToEscapedString(time) + "</span>"));
}

SystemMessageSnippet::~SystemMessageSnippet() {

}

}
