/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/Storages/CertificateFileStorage.h>

#include <iostream>
#include <boost/filesystem/fstream.hpp>

#include <Swiften/StringCodecs/SHA1.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/TLS/CertificateFactory.h>
#include <Swiften/Base/Log.h>

namespace Swift {

CertificateFileStorage::CertificateFileStorage(const boost::filesystem::path& path, CertificateFactory* certificateFactory) : path(path), certificateFactory(certificateFactory) {
}

bool CertificateFileStorage::hasCertificate(Certificate::ref certificate) const {
	boost::filesystem::path certificatePath = getCertificatePath(certificate);
	if (boost::filesystem::exists(certificatePath)) {
		ByteArray data;
		readByteArrayFromFile(data, certificatePath.string());
		Certificate::ref storedCertificate = certificateFactory->createCertificateFromDER(data);
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
	boost::filesystem::ofstream file(certificatePath, boost::filesystem::ofstream::binary|boost::filesystem::ofstream::out);
	ByteArray data = certificate->toDER();
	file.write(reinterpret_cast<const char*>(vecptr(data)), data.size());
	file.close();
}

boost::filesystem::path CertificateFileStorage::getCertificatePath(Certificate::ref certificate) const {
	return path / Hexify::hexify(SHA1::getHash(certificate->toDER()));
}

}
