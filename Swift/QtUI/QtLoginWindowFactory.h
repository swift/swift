#ifndef SWIFT_QtLoginWindowFactory_H
#define SWIFT_QtLoginWindowFactory_H

#include "Swift/Controllers/LoginWindowFactory.h"

#include <QSplitter>

namespace Swift {
	class QtLoginWindowFactory : public LoginWindowFactory{
		public:
			QtLoginWindowFactory(QSplitter* splitter);
			LoginWindow* createLoginWindow(const String& defaultJID, const String& defaultPassword, const String& defaultCertificate);
		private:
			QSplitter* splitter_;
	};
}

#endif
