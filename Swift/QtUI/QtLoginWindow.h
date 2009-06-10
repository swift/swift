#ifndef SWIFT_QtLoginWindow_H
#define SWIFT_QtLoginWindow_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QStackedWidget>

#include "Swiften/Controllers/LoginWindow.h"
#include "Swiften/Controllers/MainWindow.h"

class QLabel;
class QToolButton;

namespace Swift {
	class QtLoginWindow : public QMainWindow, public LoginWindow {
		Q_OBJECT
		public:
			QtLoginWindow(const String& defaultJID, const String& defaultPassword, const String& defaultCertificate);

			void morphInto(MainWindow *mainWindow);
			virtual void loggedOut();
			virtual void setMessage(const String& message);

		private slots:
			void loginClicked();
			void handleCertficateChecked(bool);

		private:
			QLineEdit *username_;
			QLineEdit *password_;
			QPushButton *loginButton_;
			QCheckBox *remember_;
			QStackedWidget *stack_;
			QLabel* message_;
			QString certificateFile_;
			QToolButton* certificateButton_;
	};
}

#endif
