/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMessageBox>

#include "QtMenulet.h"
#include "Slimber/MainController.h"
#include "Swiften/EventLoop/Qt/QtEventLoop.h"

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	QtEventLoop eventLoop;

	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
QMessageBox::critical(0, QObject::tr("Systray"), QObject::tr("No system tray"));
		return 1;
	}

	QtMenulet menulet;
	MainController controller(&menulet);

	return app.exec();
}
