/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QCoreApplication>
#include <QMenu>
#include <QString>
#include <QSystemTrayIcon>
#include <QObject>
#include <QPixmap>

#include "Slimber/Menulet.h"

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

		void addItem(const Swift::String& name, const Swift::String& icon) {
			menu.addAction(getIcon(icon), QString::fromUtf8(name.getUTF8Data()));
		}

		void addAboutItem() {
			menu.addAction("About");
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

		void setIcon(const Swift::String& icon) {
			trayIcon.setIcon(getIcon(icon));
		}

	private:
		QPixmap getIcon(const Swift::String& name) {
			return QPixmap(":/icons/" + QString::fromUtf8(name.getUTF8Data()) + ".png");
		}

	private slots:
		void showAboutDialog() {
		}

		void restart() {
			onRestartClicked();
		}

	private:
		QMenu menu;
		QSystemTrayIcon trayIcon;
};
