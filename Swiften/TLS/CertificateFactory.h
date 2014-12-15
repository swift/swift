/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/TLS/Certificate.h>

namespace Swift {
	class CertificateFactory {
		public:
			virtual ~CertificateFactory();

			virtual Certificate* createCertificateFromDER(const ByteArray& der) = 0;
	};
}
