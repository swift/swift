/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <string>
#include <Swiften/TLS/Certificate.h>

namespace Swift {
	/**
	 * A class to implement a check for certificate trust.
	 */
	class CertificateTrustChecker {
		public:
			virtual ~CertificateTrustChecker();

			/**
			 * This method is called to find out whether a certificate is
			 * trusted. This usually happens when a certificate's validation
			 * fails, to check whether to proceed with the connection or not.
			 */
			virtual bool isCertificateTrusted(Certificate::ref certificate) = 0;
	};
}
