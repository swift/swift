/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	class CertificateVerificationError;

	class SecurityError {
		public:
			enum Type {
				// From CertificateVerificationError
				UnknownError,
				Expired,
				NotYetValid,
				SelfSigned,
				Rejected,
				Untrusted,
				InvalidPurpose,
				PathLengthExceeded,
				InvalidSignature,
				InvalidCA,

				// Identity verification
				InvalidIdentity,
			};

			SecurityError(Type type);
			SecurityError(const CertificateVerificationError& verificationError);


			Type getType() const { 
				return type; 
			}

		private:
			Type type;
	};
}
