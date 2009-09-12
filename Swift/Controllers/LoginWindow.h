#ifndef SWIFTEN_LoginWindow_H
#define SWIFTEN_LoginWindow_H

#include "Swiften/Base/String.h"

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class MainWindow;
	class LoginWindow {
		public:
			virtual ~LoginWindow() {};
			virtual void morphInto(MainWindow *mainWindow) = 0;
			virtual void loggedOut() = 0;
			virtual void setMessage(const String&) = 0;
			virtual void addAvailableAccount(const String& defaultJID, const String& defaultPassword, const String& defaultCertificate) = 0;
			boost::signal<void (const String&, const String&, const String& /* certificateFile */, bool)> onLoginRequest;
	};
}
#endif

