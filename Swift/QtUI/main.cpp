/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QApplication>

#include "QtSwift.h"

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	bool netbookMode = argc > 1;
	Swift::QtSwift swift(netbookMode);
	return app.exec();
}
