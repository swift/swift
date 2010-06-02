/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_LoginWindow_H
#define SWIFTEN_LoginWindow_H

#include "Swiften/Base/String.h"

#include "Swiften/Base/boost_bsignals.h"
#include <boost/shared_ptr.hpp>

namespace Swift {
	class MainWindow;
	class LoginWindow {
		public:
			virtual ~LoginWindow() {};
			virtual void selectUser(const String&) = 0;
			virtual void morphInto(MainWindow *mainWindow) = 0;
			virtual void loggedOut() = 0;
			virtual void setMessage(const String&) = 0;
			virtual void setIsLoggingIn(bool loggingIn) = 0;
			virtual void addAvailableAccount(const String& defaultJID, const String& defaultPassword, const String& defaultCertificate) = 0;
			boost::signal<void (const String&, const String&, const String& /* certificateFile */, bool /* remember password*/, bool /* login automatically */)> onLoginRequest;
			virtual void setLoginAutomatically(bool loginAutomatically) = 0;
			boost::signal<void ()> onCancelLoginRequest;
	};
}
#endif

