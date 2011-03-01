/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <Swiften/Client/ClientError.h>

namespace Swift {
	class SluiftException {
		public:
			SluiftException(const std::string& reason) : reason(reason) {
			}

			SluiftException(const ClientError& error) {
				std::string reason("Disconnected: ");
				switch(error.getType()) {
					case ClientError::UnknownError: reason += "Unknown Error"; break;
					case ClientError::DomainNameResolveError: reason += "Unable to find server"; break;
					case ClientError::ConnectionError: reason += "Error connecting to server"; break;
					case ClientError::ConnectionReadError: reason += "Error while receiving server data"; break;
					case ClientError::ConnectionWriteError: reason += "Error while sending data to the server"; break;
					case ClientError::XMLError: reason += "Error parsing server data"; break;
					case ClientError::AuthenticationFailedError: reason += "Login/password invalid"; break;
					case ClientError::CompressionFailedError: reason += "Error while compressing stream"; break;
					case ClientError::ServerVerificationFailedError: reason += "Server verification failed"; break;
					case ClientError::NoSupportedAuthMechanismsError: reason += "Authentication mechanisms not supported"; break;
					case ClientError::UnexpectedElementError: reason += "Unexpected response"; break;
					case ClientError::ResourceBindError: reason += "Error binding resource"; break;
					case ClientError::SessionStartError: reason += "Error starting session"; break;
					case ClientError::StreamError: reason += "Stream error"; break;
					case ClientError::TLSError: reason += "Encryption error"; break;
					case ClientError::ClientCertificateLoadError: reason += "Error loading certificate (Invalid password?)"; break;
					case ClientError::ClientCertificateError: reason += "Certificate not authorized"; break;
					case ClientError::UnknownCertificateError: reason += "Unknown certificate"; break;
					case ClientError::CertificateExpiredError: reason += "Certificate has expired"; break;
					case ClientError::CertificateNotYetValidError: reason += "Certificate is not yet valid"; break;
					case ClientError::CertificateSelfSignedError: reason += "Certificate is self-signed"; break;
					case ClientError::CertificateRejectedError: reason += "Certificate has been rejected"; break;
					case ClientError::CertificateUntrustedError: reason += "Certificate is not trusted"; break;
					case ClientError::InvalidCertificatePurposeError: reason += "Certificate cannot be used for encrypting your connection"; break;
					case ClientError::CertificatePathLengthExceededError: reason += "Certificate path length constraint exceeded"; break;
					case ClientError::InvalidCertificateSignatureError: reason += "Invalid certificate signature"; break;
					case ClientError::InvalidCAError: reason += "Invalid Certificate Authority"; break;
					case ClientError::InvalidServerIdentityError: reason += "Certificate does not match the host identity"; break;
				}
			}

			const std::string& getReason() const {
				return reason;
			}

		private:
			std::string reason;
	};
}
