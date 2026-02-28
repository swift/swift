/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/signals2.hpp>

#include <Swiften/Client/ClientOptions.h>
#include <Swiften/TLS/Certificate.h>
#include <Swiften/TLS/CertificateWithKey.h>

namespace Swift {
    class MainWindow;
    class LoginWindow {
        public:
            virtual ~LoginWindow() {}
            virtual void selectUser(const std::string&) = 0;
            virtual void morphInto(MainWindow *mainWindow) = 0;
            virtual void loggedOut() = 0;
            virtual void setShowNotificationToggle(bool) = 0;
            virtual void setMessage(const std::string&) = 0;
            virtual void setIsLoggingIn(bool loggingIn) = 0;
            virtual void addAvailableAccount(const std::string& defaultJID, const std::string& defaultPassword, const std::string& defaultCertificate, const ClientOptions& options) = 0;
            virtual void removeAvailableAccount(const std::string& jid) = 0;
            /** The certificate is what is used for login, the certificatePath is used for remembering paths to populate the loginwindow with*/
            boost::signals2::signal<void (const std::string&, const std::string&, const std::string& /*CertificatePath*/, CertificateWithKey::ref /* clientCertificate */, const ClientOptions& /*options*/, bool /* remember password*/, bool /* login automatically */)> onLoginRequest;
            virtual void setLoginAutomatically(bool loginAutomatically) = 0;
            virtual void quit() = 0;
            /** Blocking request whether a cert should be permanently trusted.*/
            virtual bool askUserToTrustCertificatePermanently(const std::string& message, const std::vector<Certificate::ref>& certificateChain) = 0;

            boost::signals2::signal<void ()> onCancelLoginRequest;
            boost::signals2::signal<void ()> onQuitRequest;
            boost::signals2::signal<void (const std::string&)> onPurgeSavedLoginRequest;
    };
}
