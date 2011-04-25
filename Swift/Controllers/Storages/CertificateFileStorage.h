/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>

#include "Swift/Controllers/Storages/CertificateStorage.h"

namespace Swift {
	class CertificateFactory;

	class CertificateFileStorage : public CertificateStorage {
		public:
			CertificateFileStorage(const boost::filesystem::path& path, CertificateFactory* certificateFactory);

			virtual bool hasCertificate(Certificate::ref certificate) const;
			virtual void addCertificate(Certificate::ref certificate);

		private:
			boost::filesystem::path getCertificatePath(Certificate::ref certificate) const;

		private:
			boost::filesystem::path path;
			CertificateFactory* certificateFactory;
	};

}
