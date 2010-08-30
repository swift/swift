/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "MessageSnippet.h"

#include <QtDebug>
#include <QDateTime>

namespace Swift {

MessageSnippet::MessageSnippet(const QString& message, const QString& sender, const QDateTime& time, const QString& iconURI, bool isIncoming, bool appendToPrevious, QtChatTheme* theme) : ChatSnippet(appendToPrevious) {
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

	content_.replace("%message%", "<span class='swift_message'>" + escape(message) + "</span>");
	content_.replace("%sender%", escape(sender));
	content_.replace("%time%", "<span class='swift_time'>" + escape(time.toString("h:mm")) + "</span>");
	content_.replace("%userIconPath%", escape(iconURI));
}

MessageSnippet::~MessageSnippet() {

}

}
