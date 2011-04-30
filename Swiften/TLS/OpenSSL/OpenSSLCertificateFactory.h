/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/TLS/CertificateFactory.h>
#include <Swiften/TLS/OpenSSL/OpenSSLCertificate.h>

namespace Swift {
	class OpenSSLCertificateFactory : public CertificateFactory {
		public:
			virtual Certificate::ref createCertificateFromDER(const ByteArray& der) {
				return Certificate::ref(new OpenSSLCertificate(der));
			}
	};
}
