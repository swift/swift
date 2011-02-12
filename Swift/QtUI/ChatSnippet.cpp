/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QFile>

#include "ChatSnippet.h"

namespace Swift {

ChatSnippet::ChatSnippet(bool appendToPrevious) : appendToPrevious_(appendToPrevious) {
}

ChatSnippet::~ChatSnippet() {
}

QString ChatSnippet::timeToEscapedString(const QDateTime& time) {
	QDate now(QDate::currentDate());
	QString date = "";
	if (time.date().daysTo(now) > 0) {
		date = "ddd ";
	}
	if (time.date().month() != now.month()) {
		date = date + "MMMM ";
	}
	if (time.date().daysTo(now) > 6) {
		date = date + "d ";
	}
	if (time.date().year() != now.year()) {
		date = date + "yy ";
	}
	date += "h:mm";
	return escape(time.toString(date));
}

};
