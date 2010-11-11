/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/Error.h"

namespace Swift {
	class CertificateVerificationError : public Error {
		public:
			typedef boost::shared_ptr<CertificateVerificationError> ref;

			enum Type {
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
				InvalidServerIdentity,
			};

			CertificateVerificationError(Type type = UnknownError) : type(type) {}

			Type getType() const { 
				return type; 
			}

		private:
			Type type;
	};
}
