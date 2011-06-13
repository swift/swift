/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QApplication>
#include <QCoreApplication>
#include <QSystemTrayIcon>
#include <QMessageBox>

#include "QtMenulet.h"
#include "Slimber/MainController.h"
#include <Swiften/EventLoop/Qt/QtEventLoop.h>
#include <Slimber/Qt/BuildVersion.h>

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	Swift::QtEventLoop eventLoop;

	QCoreApplication::setApplicationName("Slimber");
	QCoreApplication::setApplicationVersion(QString(buildVersion));

	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
QMessageBox::critical(0, QObject::tr("Systray"), QObject::tr("No system tray"));
		return 1;
	}

	app.setQuitOnLastWindowClosed(false);

	QtMenulet menulet;
	MainController controller(&menulet, &eventLoop);

	return app.exec();
}
