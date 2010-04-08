/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "MessageSnippet.h"

#include <QtDebug>
#include <QDateTime>

namespace Swift {

MessageSnippet::MessageSnippet(const QString& message, const QString& sender, const QDateTime& time, const QString& iconURI, bool isIncoming, bool appendToPrevious) : ChatSnippet(appendToPrevious) {
	if (isIncoming) {
		if (appendToPrevious) {
			content_ = loadTemplate(":/themes/Default/Incoming/NextContent.html");
		}
		else {
			content_ = loadTemplate(":/themes/Default/Incoming/Content.html");
		}
	}
	else {
		if (appendToPrevious) {
			content_ = loadTemplate(":/themes/Default/Outgoing/NextContent.html");
		}
		else {
			content_ = loadTemplate(":/themes/Default/Outgoing/Content.html");
		}
	}

	content_.replace("%message%", escape(message));
	content_.replace("%sender%", escape(sender));
	content_.replace("%time%", escape(time.toString("h:mm")));
	content_.replace("%userIconPath%", escape(iconURI));
}

}
