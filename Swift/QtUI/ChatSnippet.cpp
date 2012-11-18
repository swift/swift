/*
 * Copyright (c) 2010-2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QFile>

#include "ChatSnippet.h"
#include <Swift/QtUI/QtSwiftUtil.h>

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

QString ChatSnippet::wrapResizable(const QString& text) {
	return "<span class='swift_resizable'>" + text + "</span>";
}

QString ChatSnippet::directionToCSS(Direction direction) {
	return direction == RTL ? QString("rtl") : QString("ltr");
}

ChatSnippet::Direction ChatSnippet::getDirection(const std::string& message) {
	return getDirection(P2QSTRING(message));
}

ChatSnippet::Direction ChatSnippet::getDirection(const QString& message) {
	/*
	for (int i = 0; i < message.size(); ++i) {
		switch (message.at(i).direction()) {
			case QChar::DirL:
			case QChar::DirLRE:
			case QChar::DirLRO:
				return ChatSnippet::LTR;
			case QChar::DirR:
			case QChar::DirAL:
			case QChar::DirRLE:
			case QChar::DirRLO:
				return ChatSnippet::RTL;
			case QChar::DirEN:
			case QChar::DirES:
			case QChar::DirET:
			case QChar::DirAN:
			case QChar::DirCS:
			case QChar::DirB:
			case QChar::DirWS:
			case QChar::DirON:
			case QChar::DirS:
			case QChar::DirPDF:
			case QChar::DirNSM:
			case QChar::DirBN:
				break;
		}
	}
	return ChatSnippet::LTR;
	*/
	return message.isRightToLeft() ? ChatSnippet::RTL : ChatSnippet::LTR;
}


}
