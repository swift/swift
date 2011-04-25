/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/Storages/CertificateStorageFactory.h>
#include <Swift/Controllers/Storages/CertificateFileStorage.h>

namespace Swift {
	class CertificateFactory;

	class CertificateFileStorageFactory : public CertificateStorageFactory {
		public:
			CertificateFileStorageFactory(const boost::filesystem::path& basePath, CertificateFactory* certificateFactory) : basePath(basePath), certificateFactory(certificateFactory) {}

			virtual CertificateStorage* createCertificateStorage(const JID& profile) const {
				boost::filesystem::path profilePath = basePath / profile.toString();
				return new CertificateFileStorage(profilePath / "certificates", certificateFactory);
			}

		private:
			boost::filesystem::path basePath;
			CertificateFactory* certificateFactory;
	};
}
