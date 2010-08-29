/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "SystemMessageSnippet.h"

#include <QDateTime>

namespace Swift {

SystemMessageSnippet::SystemMessageSnippet(const QString& message, const QDateTime& time, bool appendToPrevious, QtChatTheme* theme) : ChatSnippet(appendToPrevious) {
	content_ = theme->getStatus();

	content_.replace("%message%", escape(message));
	content_.replace("%shortTime%", escape(time.toString("h:mm")));
	content_.replace("%time%", escape(time.toString("h:mm")));
}

SystemMessageSnippet::~SystemMessageSnippet() {

}

}
