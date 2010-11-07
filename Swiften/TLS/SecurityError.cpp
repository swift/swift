/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/TLS/SecurityError.h"
#include "Swiften/TLS/CertificateVerificationError.h"

namespace Swift {

SecurityError::SecurityError(Type type) : type(type) {
}

SecurityError::SecurityError(const CertificateVerificationError& verificationError) {
	type = UnknownError;
	switch(verificationError.getType()) {
		case CertificateVerificationError::UnknownError: type = UnknownError;
		case CertificateVerificationError::Expired: type = Expired;
		case CertificateVerificationError::NotYetValid: type = NotYetValid;
		case CertificateVerificationError::SelfSigned: type = SelfSigned;
		case CertificateVerificationError::Rejected: type = Rejected;
		case CertificateVerificationError::Untrusted: type = Untrusted;
		case CertificateVerificationError::InvalidPurpose: type = InvalidPurpose;
		case CertificateVerificationError::PathLengthExceeded: type = PathLengthExceeded;
		case CertificateVerificationError::InvalidSignature: type = InvalidSignature;
		case CertificateVerificationError::InvalidCA: type = InvalidCA;
	}
}

}
