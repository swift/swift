/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/TLS/Certificate.h>

namespace Swift {
	/**
	 * A class to implement a check for certificate trust.
	 */
	class SWIFTEN_API CertificateTrustChecker {
		public:
			virtual ~CertificateTrustChecker();

			/**
			 * This method is called to find out whether a certificate (chain) is
			 * trusted. This usually happens when a certificate's validation
			 * fails, to check whether to proceed with the connection or not.
			 *
			 * certificateChain contains the chain of certificates. The first certificate
			 * is the subject certificate.
			 */
			virtual bool isCertificateTrusted(const std::vector<Certificate::ref>& certificateChain) = 0;
	};
}
