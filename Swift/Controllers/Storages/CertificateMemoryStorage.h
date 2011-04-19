/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <Swift/Controllers/Storages/CertificateStorage.h>

namespace Swift {
	class CertificateMemoryStorage : public CertificateStorage {
		public:
			CertificateMemoryStorage();

			virtual bool hasCertificate(Certificate::ref certificate) const;
			virtual void addCertificate(Certificate::ref certificate);

		private:
			std::vector<Certificate::ref> certificates;
	};

}
