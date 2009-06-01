#include "QtLoginWindowFactory.h"
#include "QtLoginWindow.h"

namespace Swift {
LoginWindow* QtLoginWindowFactory::createLoginWindow(const String& defaultJID, const String& defaultPassword, const String& defaultCertificate) {
	return new QtLoginWindow(defaultJID, defaultPassword, defaultCertificate);
}
}
