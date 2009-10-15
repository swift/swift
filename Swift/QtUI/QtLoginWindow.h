#ifndef SWIFT_QtLoginWindow_H
#define SWIFT_QtLoginWindow_H

#include <QMainWindow>
#include <QPointer>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QStackedWidget>
#include <QMenuBar>

#include "Swift/Controllers/LoginWindow.h"
#include "Swift/Controllers/MainWindow.h"
#include "QtAboutWidget.h"

class QLabel;
class QToolButton;
class QComboBox;

namespace Swift {
	class QtLoginWindow : public QMainWindow, public LoginWindow {
		Q_OBJECT
		public:
			QtLoginWindow();

			void morphInto(MainWindow *mainWindow);
			virtual void loggedOut();
			virtual void setMessage(const String& message);
			virtual void addAvailableAccount(const String& defaultJID, const String& defaultPassword, const String& defaultCertificate);
			static QRect defaultPosition();
			void setGentleGeometry(const QRect&);
		signals:
			void geometryChanged();

		private slots:
			void loginClicked();
			void handleCertficateChecked(bool);
			void handleQuit();
			void handleAbout();
			void bringToFront();
			void handleUsernameTextChanged();
			void resizeEvent(QResizeEvent* event);
			void moveEvent(QMoveEvent* event);

		private:
			void setInitialMenus();
			QStringList usernames_;
			QStringList passwords_;
			QStringList certificateFiles_;
			QComboBox* username_;
			QLineEdit* password_;
			QPushButton* loginButton_;
			QCheckBox* remember_;
			QStackedWidget* stack_;
			QLabel* message_;
			QString certificateFile_;
			QToolButton* certificateButton_;
			QMenuBar* menuBar_;
			QMenu* swiftMenu_;
			QPointer<QtAboutWidget> aboutDialog_;
	};
}

#endif
