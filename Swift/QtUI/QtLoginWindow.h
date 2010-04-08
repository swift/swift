/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFT_QtLoginWindow_H
#define SWIFT_QtLoginWindow_H

#include <QMainWindow>
#include <QPointer>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QStackedWidget>
#include <QMenuBar>

#include "Swift/Controllers/UIInterfaces/LoginWindow.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/MainWindow.h"
#include "QtAboutWidget.h"

class QLabel;
class QToolButton;
class QComboBox;

namespace Swift {
	class QtLoginWindow : public QMainWindow, public LoginWindow {
		Q_OBJECT
		public:
			QtLoginWindow(UIEventStream* uiEventStream);

			void morphInto(MainWindow *mainWindow);
			virtual void loggedOut();
			virtual void setMessage(const String& message);
			virtual void addAvailableAccount(const String& defaultJID, const String& defaultPassword, const String& defaultCertificate);
			virtual void setLoginAutomatically(bool loginAutomatically);
			virtual void setIsLoggingIn(bool loggingIn);
			static QRect defaultPosition();
			void setGentleGeometry(const QRect&);
			void selectUser(const String& user);
		signals:
			void geometryChanged();

		private slots:
			void loginClicked();
			void handleCertficateChecked(bool);
			void handleQuit();
			void handleShowXMLConsole();
			void handleToggleSounds(bool enabled);
			void handleAbout();
			void bringToFront();
			void handleUsernameTextChanged();
			void resizeEvent(QResizeEvent* event);
			void moveEvent(QMoveEvent* event);
			void handleUIEvent(boost::shared_ptr<UIEvent> event);

		private:
			void setInitialMenus();
			QStringList usernames_;
			QStringList passwords_;
			QStringList certificateFiles_;
			QComboBox* username_;
			QLineEdit* password_;
			QPushButton* loginButton_;
			QCheckBox* remember_;
			QCheckBox* loginAutomatically_;
			QStackedWidget* stack_;
			QLabel* message_;
			QString certificateFile_;
			QToolButton* certificateButton_;
			QMenuBar* menuBar_;
			QMenu* swiftMenu_;
			QMenu* toolsMenu_;
			QAction* toggleSoundsAction_;
			UIEventStream* uiEventStream_;
			QPointer<QtAboutWidget> aboutDialog_;
	};
}

#endif
