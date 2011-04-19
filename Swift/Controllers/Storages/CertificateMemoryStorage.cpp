/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/Storages/CertificateMemoryStorage.h>

#include <Swiften/Base/foreach.h>

using namespace Swift;

CertificateMemoryStorage::CertificateMemoryStorage() {
}

bool CertificateMemoryStorage::hasCertificate(Certificate::ref certificate) const {
	foreach(Certificate::ref storedCert, certificates) {
		if (storedCert->toDER() == certificate->toDER()) {
			return true;
		}
	}
	return false;
}

void CertificateMemoryStorage::addCertificate(Certificate::ref certificate) {
	certificates.push_back(certificate);
}
