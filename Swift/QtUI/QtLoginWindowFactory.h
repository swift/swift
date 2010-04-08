/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFT_QtLoginWindowFactory_H
#define SWIFT_QtLoginWindowFactory_H

#include "Swift/Controllers/UIInterfaces/LoginWindowFactory.h"
#include "Swift/QtUI/QtSystemTray.h"

#include <QSplitter>

#include "QtSettingsProvider.h"

namespace Swift {
	class QtLoginWindow;
	class QtLoginWindowFactory : public QObject, public LoginWindowFactory {
		Q_OBJECT
		public:
			QtLoginWindowFactory(QSplitter* splitter, QtSystemTray* systemTray, QtSettingsProvider* settings);
			LoginWindow* createLoginWindow(UIEventStream* uiEventStream);
		private slots:
			void handleWindowGeometryChanged();
		private:
			QSplitter* splitter_;
			QtSystemTray* systemTray_;
			QtSettingsProvider* settings_;
			QtLoginWindow* window_;
	};
}

#endif
