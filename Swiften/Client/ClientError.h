/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/system/system_error.hpp>

namespace Swift {
	class ClientError {
		public:
			enum Type {
				UnknownError,
				DomainNameResolveError,
				ConnectionError,
				ConnectionReadError,
				ConnectionWriteError,
				XMLError,
				AuthenticationFailedError,
				CompressionFailedError,
				ServerVerificationFailedError,
				NoSupportedAuthMechanismsError,
				UnexpectedElementError,
				ResourceBindError,
				SessionStartError,
				StreamError,
				TLSError,
				ClientCertificateLoadError,
				ClientCertificateError,

				// Certifate on smartcard was removed
				CertificateCardRemoved,

				// Certificate verification errors
				UnknownCertificateError,
				CertificateExpiredError,
				CertificateNotYetValidError,
				CertificateSelfSignedError,
				CertificateRejectedError,
				CertificateUntrustedError,
				InvalidCertificatePurposeError,
				CertificatePathLengthExceededError,
				InvalidCertificateSignatureError,
				InvalidCAError,
				InvalidServerIdentityError,
				RevokedError,
				RevocationCheckFailedError
			};

			ClientError(Type type = UnknownError) : type_(type) {}

			Type getType() const { return type_; }

			void setErrorCode(boost::shared_ptr<boost::system::error_code> errorCode) { errorCode_ = errorCode; }

			boost::shared_ptr<boost::system::error_code> getErrorCode() const { return errorCode_; }

		private:
			Type type_;
			boost::shared_ptr<boost::system::error_code> errorCode_;
	};
}
