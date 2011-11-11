/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "MessageSnippet.h"

#include <QtDebug>
#include <QDateTime>

namespace Swift {

MessageSnippet::MessageSnippet(const QString& message, const QString& sender, const QDateTime& time, const QString& iconURI, bool isIncoming, bool appendToPrevious, QtChatTheme* theme, const QString& id) : ChatSnippet(appendToPrevious) {
	if (appendToPrevious) {
		setContinuationFallbackSnippet(boost::shared_ptr<ChatSnippet>(new MessageSnippet(message, sender, time, iconURI, isIncoming, false, theme, id)));
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

	content_.replace("%message%", "<span class='swift_message'>" + escape(message) + "</span><span class='swift_ack'></span><span class='swift_receipt'></span>");
	content_.replace("%sender%", escape(sender));
	content_.replace("%time%", "<span class='swift_time'>" + timeToEscapedString(time) + "</span>");
	content_.replace("%userIconPath%", escape(iconURI));
	content_ = "<div id='" + id + "'>" + content_ + "</div>";
}

MessageSnippet::~MessageSnippet() {

}

}
