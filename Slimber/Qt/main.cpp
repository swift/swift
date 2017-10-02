/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <QApplication>
#include <QCoreApplication>
#include <QMessageBox>
#include <QSystemTrayIcon>

#include <Swiften/EventLoop/Qt/QtEventLoop.h>

#include <Slimber/MainController.h>
#include <Slimber/Qt/BuildVersion.h>
#include <Slimber/Qt/QtMenulet.h>

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
