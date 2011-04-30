/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/TLS/CertificateTrustChecker.h>

namespace Swift {
	/**
	 * A certificate trust checker that trusts any ceritficate.
	 *
	 * This can be used to ignore any TLS certificate errors occurring 
	 * during connection.
	 *
	 * \see Client::setAlwaysTrustCertificates()
	 */
	class BlindCertificateTrustChecker : public CertificateTrustChecker {
		public:
			virtual bool isCertificateTrusted(Certificate::ref) {
				return true;
			}
	};
}
