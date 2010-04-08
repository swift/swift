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

QString ChatSnippet::loadTemplate(const QString& filename) {
	QFile file(filename);
	bool result = file.open(QIODevice::ReadOnly);
	Q_ASSERT(result);
	Q_UNUSED(result);
	QString content = file.readAll();
	file.close();
	return content;
}

QString ChatSnippet::escape(const QString& original) {
	QString result(original);
	result.replace("%message%", "&#37;message&#37;");
	result.replace("%sender%", "&#37;sender&#37;");
	result.replace("%time%", "%&#37;time&#37;");
	result.replace("%shortTime%", "%&#37;shortTime&#37;");
	result.replace("%userIconPath%", "&#37;userIconPath&#37;");
	return result;
}

};
