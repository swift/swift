#ifndef SWIFT_QtLoginWindowFactory_H
#define SWIFT_QtLoginWindowFactory_H

#include "Swift/Controllers/LoginWindowFactory.h"
#include "Swift/QtUI/QtSystemTray.h"

#include <QSplitter>

namespace Swift {
	class QtLoginWindowFactory : public QObject, public LoginWindowFactory {
		Q_OBJECT
		public:
			QtLoginWindowFactory(QSplitter* splitter, QtSystemTray* systemTray);
			LoginWindow* createLoginWindow(const String& defaultJID, const String& defaultPassword, const String& defaultCertificate);
		private:
			QSplitter* splitter_;
			QtSystemTray* systemTray_;
	};
}

#endif
