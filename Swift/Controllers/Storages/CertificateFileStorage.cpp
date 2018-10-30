/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Storages/CertificateFileStorage.h>

#include <iostream>

#include <boost/filesystem/fstream.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/TLS/CertificateFactory.h>

namespace Swift {

CertificateFileStorage::CertificateFileStorage(const boost::filesystem::path& path, CertificateFactory* certificateFactory, CryptoProvider* crypto) : path(path), certificateFactory(certificateFactory), crypto(crypto) {
}

bool CertificateFileStorage::hasCertificate(Certificate::ref certificate) const {
    boost::filesystem::path certificatePath = getCertificatePath(certificate);
    if (boost::filesystem::exists(certificatePath)) {
        ByteArray data;
        readByteArrayFromFile(data, certificatePath);
        Certificate::ref storedCertificate(certificateFactory->createCertificateFromDER(data));
        if (storedCertificate && storedCertificate->toDER() == certificate->toDER()) {
            return true;
        }
        else {
            SWIFT_LOG(warning) << "Stored certificate does not match received certificate" << std::endl;
            return false;
        }
    }
    else {
        return false;
    }
}

void CertificateFileStorage::addCertificate(Certificate::ref certificate) {
    boost::filesystem::path certificatePath = getCertificatePath(certificate);
    if (!boost::filesystem::exists(certificatePath.parent_path())) {
        try {
            boost::filesystem::create_directories(certificatePath.parent_path());
        }
        catch (const boost::filesystem::filesystem_error& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
        }
    }
    try {
        boost::filesystem::ofstream file(certificatePath, boost::filesystem::ofstream::binary|boost::filesystem::ofstream::out);
        ByteArray data = certificate->toDER();
        file.write(reinterpret_cast<const char*>(vecptr(data)), boost::numeric_cast<std::streamsize>(data.size()));
        file.close();
    }
    catch (...) {
        SWIFT_LOG(warning) << "Failed to store certificate to " << certificatePath << std::endl;
    }
}

boost::filesystem::path CertificateFileStorage::getCertificatePath(Certificate::ref certificate) const {
    return path / Hexify::hexify(crypto->getSHA1Hash(certificate->toDER()));
}

}
