#ifndef SWIFT_QtLoginWindowFactory_H
#define SWIFT_QtLoginWindowFactory_H

#include "Swiften/Controllers/LoginWindowFactory.h"

namespace Swift {
	class QtLoginWindowFactory : public LoginWindowFactory{
		LoginWindow* createLoginWindow(const String& defaultJID, const String& defaultPassword, const String& defaultCertificate);
	};
}

#endif
