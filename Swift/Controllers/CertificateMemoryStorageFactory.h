/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/Storages/CertificateStorageFactory.h>
#include <Swift/Controllers/Storages/CertificateMemoryStorage.h>

namespace Swift {
	class CertificateFactory;

	class CertificateMemoryStorageFactory : public CertificateStorageFactory {
		public:
			CertificateMemoryStorageFactory() {
			}

			virtual CertificateStorage* createCertificateStorage(const JID&) const {
				return new CertificateMemoryStorage();
			}

		private:
			boost::filesystem::path basePath;
			CertificateFactory* certificateFactory;
	};
}
