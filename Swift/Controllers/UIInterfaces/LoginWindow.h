/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>

#include <string>
#include <Swiften/TLS/Certificate.h>
#include <Swiften/TLS/CertificateWithKey.h>

namespace Swift {
	class MainWindow;
	class LoginWindow {
		public:
			virtual ~LoginWindow() {};
			virtual void selectUser(const std::string&) = 0;
			virtual void morphInto(MainWindow *mainWindow) = 0;
			virtual void loggedOut() = 0;
			virtual void setShowNotificationToggle(bool) = 0;
			virtual void setMessage(const std::string&) = 0;
			virtual void setIsLoggingIn(bool loggingIn) = 0;
			virtual void addAvailableAccount(const std::string& defaultJID, const std::string& defaultPassword, const std::string& defaultCertificate) = 0;
			virtual void removeAvailableAccount(const std::string& jid) = 0;
			/** The certificate is what is used for login, the certificatePath is used for remembering paths to populate the loginwindow with*/
			boost::signal<void (const std::string&, const std::string&, const std::string& /*CertificatePath*/, CertificateWithKey::ref /* clientCertificate */, bool /* remember password*/, bool /* login automatically */)> onLoginRequest;
			virtual void setLoginAutomatically(bool loginAutomatically) = 0;
			virtual void quit() = 0;
			/** Blocking request whether a cert should be permanently trusted.*/
			virtual bool askUserToTrustCertificatePermanently(const std::string& message, Certificate::ref) = 0;

			boost::signal<void ()> onCancelLoginRequest;
			boost::signal<void ()> onQuitRequest;
			boost::signal<void (const std::string&)> onPurgeSavedLoginRequest;
	};
}
