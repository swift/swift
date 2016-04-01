/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#define SECURITY_WIN32
#include <Windows.h>
#include <Sspi.h>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/SASL/ClientAuthenticator.h>

namespace Swift {
    class SWIFTEN_API WindowsGSSAPIClientAuthenticator : public ClientAuthenticator {
        public:
            WindowsGSSAPIClientAuthenticator(const std::string& hostname, const std::string& domainname, int port);

            virtual ~WindowsGSSAPIClientAuthenticator();

            virtual boost::optional<SafeByteArray> getResponse() const;
            virtual bool setChallenge(const boost::optional<std::vector<unsigned char> >&);

            /**
              * Returns whether the authenticator has the complete
              * security context. It could be true before any
              * message exchanges with the server or after some
              * messages have been exchanged.
              *
              * @return True if security context is complete.
              */
            bool isCompleteContext() {
                return haveCompleteContext_;
            }

            /**
              * Retrieves the user name associated with the
              * security context. It will only be set when
              * isCompleteContext() returns true.
              *
              * @return User name in the form "EXAMPLE.COM\user".
              */
            const std::string& getUserName() {
                return userName_;
            }

            /**
              * Retrieves the client part of the user name
              * associated with the security context. It will only
              * be set when isCompleteContext() returns true.
              *
              * @return Client name in the form "user" when the user
              * name is "EXAMPLE.COM\user".
              */
            const std::string& getClientName() {
                return clientName_;
            }

            /**
              * Retrieves the server name associated with the
              * security context. It will only be set when
              * isCompleteContext() returns true.
              *
              * @return Server name in the form "EXAMPLE.COM".
              */
            const std::string& getServerName() {
                return serverName_;
            }

            /**
              * Returns whether an error has occurred at any point,
              * including in the constructor.
              *
              * @return True if an error has occured.
              */
            bool isError();

            /**
              * Returns error details if isError() returns true.
              * May be empty if there are no details to be provided
              * for the error.
              *
              * @return Error details.
              */
            std::shared_ptr<boost::system::error_code> getErrorCode() {
                return errorCode_;
            }

        private:
            void buildSecurityContext(const boost::optional<ByteArray>& inputToken);

        private:
            enum Step {
                BuildingSecurityContext,
                SecurityLayerNegotiation,
                ServerAuthenticated
            } step_;
            bool error_;
            std::shared_ptr<boost::system::error_code> errorCode_;
            std::string servicePrincipalNameString_;
            bool haveCredentialsHandle_;
            bool haveContextHandle_;
            bool haveCompleteContext_;
            CredHandle credentialsHandle_;
            CtxtHandle contextHandle_;
            SecPkgContext_Sizes sizes_;
            SecPkgContext_StreamSizes streamSizes_;
            std::string userName_;
            std::string clientName_;
            std::string serverName_;
            SafeByteArray response_;
    };
}
