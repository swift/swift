/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Storages/CertificateMemoryStorage.h>

using namespace Swift;

CertificateMemoryStorage::CertificateMemoryStorage() {
}

bool CertificateMemoryStorage::hasCertificate(Certificate::ref certificate) const {
    for (auto&& storedCert : certificates) {
        if (storedCert->toDER() == certificate->toDER()) {
            return true;
        }
    }
    return false;
}

void CertificateMemoryStorage::addCertificate(Certificate::ref certificate) {
    certificates.push_back(certificate);
}
