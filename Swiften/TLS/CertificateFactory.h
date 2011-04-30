/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/TLS/Certificate.h>

namespace Swift {
	class CertificateFactory {
		public:
			virtual ~CertificateFactory();

			virtual Certificate::ref createCertificateFromDER(const ByteArray& der) = 0;
	};
}
