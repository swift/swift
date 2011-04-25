/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/TLS/CertificateTrustChecker.h>
#include <Swift/Controllers/Storages/CertificateStorage.h>

namespace Swift {
	/**
	 * A certificate trust checker that trusts certificates in a certificate storage.
	 */
	class CertificateStorageTrustChecker : public CertificateTrustChecker {
		public:
			CertificateStorageTrustChecker(CertificateStorage* storage) : storage(storage) {
			}

			virtual bool isCertificateTrusted(Certificate::ref certificate) {
				lastCertificate = certificate;
				return storage->hasCertificate(certificate);
			}

			Certificate::ref getLastCertificate() const {
				return lastCertificate;
			}

		private:
			CertificateStorage* storage;
			Certificate::ref lastCertificate;
	};
}
