/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>

#include <Swiften/TLS/Certificate.h>

namespace Swift {
	class CertificateStorage {
		public:
			virtual ~CertificateStorage();

			virtual bool hasCertificate(Certificate::ref certificate) const = 0;
			virtual void addCertificate(Certificate::ref certificate) = 0;
	};

}
