/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/TLS/CertificateTrustChecker.h"

namespace Swift {
	class BlindCertificateTrustChecker : public CertificateTrustChecker {
		public:
			virtual bool isCertificateTrusted(Certificate::ref, const String&) {
				return true;
			}
	};
}
