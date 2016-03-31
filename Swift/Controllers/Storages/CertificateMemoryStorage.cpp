/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
