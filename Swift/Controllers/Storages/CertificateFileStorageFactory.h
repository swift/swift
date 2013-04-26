/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/Storages/CertificateStorageFactory.h>
#include <Swift/Controllers/Storages/CertificateFileStorage.h>

namespace Swift {
	class CertificateFactory;
	class CryptoProvider;

	class CertificateFileStorageFactory : public CertificateStorageFactory {
		public:
			CertificateFileStorageFactory(const boost::filesystem::path& basePath, CertificateFactory* certificateFactory, CryptoProvider* crypto) : basePath(basePath), certificateFactory(certificateFactory), crypto(crypto) {}

			virtual CertificateStorage* createCertificateStorage(const JID& profile) const {
				boost::filesystem::path profilePath = basePath / profile.toString();
				return new CertificateFileStorage(profilePath / "certificates", certificateFactory, crypto);
			}

		private:
			boost::filesystem::path basePath;
			CertificateFactory* certificateFactory;
			CryptoProvider* crypto;
	};
}
