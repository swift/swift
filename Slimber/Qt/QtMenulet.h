/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QCoreApplication>
#include <QMenu>
#include <QObject>
#include <QPixmap>
#include <QPointer>
#include <QString>
#include <QSystemTrayIcon>

#include <Slimber/Menulet.h>
#include <Slimber/Qt/QtAboutDialog.h>

class QtMenulet : public QObject, public Menulet {
        Q_OBJECT
    public:
        QtMenulet() {
            trayIcon.setIcon(QPixmap(":/icons/UsersOffline.png"));
            trayIcon.setContextMenu(&menu);
            trayIcon.show();
        }

        void clear() {
            menu.clear();
        }

        void addItem(const std::string& name, const std::string& icon) {
            menu.addAction(getIcon(icon), QString::fromUtf8(name.c_str()));
        }

        void addAboutItem() {
            menu.addAction("About", this, SLOT(showAboutDialog()));
        }

        void addRestartItem() {
            menu.addAction("Restart", this, SLOT(restart()));
        }

        void addExitItem() {
            menu.addAction("Exit", qApp, SLOT(quit()));
        }

        void addSeparator() {
            menu.addSeparator();
        }

        void setIcon(const std::string& icon) {
            trayIcon.setIcon(getIcon(icon));
        }

    private:
        QPixmap getIcon(const std::string& name) {
            return QPixmap(":/icons/" + QString::fromUtf8(name.c_str()) + ".png");
        }

    private slots:
        void showAboutDialog() {
            if (aboutDialog) {
                aboutDialog->raise();
                aboutDialog->activateWindow();
            }
            else {
                aboutDialog = new QtAboutDialog();
                aboutDialog->show();
            }
        }

        void restart() {
            onRestartClicked();
        }

    private:
        QMenu menu;
        QSystemTrayIcon trayIcon;
        QPointer<QtAboutDialog> aboutDialog;
};
