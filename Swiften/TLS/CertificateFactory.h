/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/TLS/Certificate.h>

namespace Swift {
	class SWIFTEN_API CertificateFactory {
		public:
			virtual ~CertificateFactory();

			virtual Certificate* createCertificateFromDER(const ByteArray& der) = 0;
	};
}
