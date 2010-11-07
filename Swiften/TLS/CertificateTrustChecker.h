/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/TLS/Certificate.h"

namespace Swift {
	class CertificateTrustChecker {
		public:
			virtual ~CertificateTrustChecker();

			virtual bool isCertificateTrusted(Certificate::ref certificate, const String& domain) = 0;
	};
}
