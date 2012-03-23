/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

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

		private:
			Type type_;
	};
}
