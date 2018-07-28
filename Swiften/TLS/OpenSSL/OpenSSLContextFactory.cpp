/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/TLS/OpenSSL/OpenSSLContextFactory.h>

#include <openssl/bio.h>
#include <openssl/dh.h>
#include <openssl/pem.h>

#include <Swiften/Base/Log.h>
#include <Swiften/TLS/OpenSSL/OpenSSLContext.h>

#pragma clang diagnostic ignored "-Wshorten-64-to-32"

namespace Swift {

bool OpenSSLContextFactory::canCreate() const {
    return true;
}

std::unique_ptr<TLSContext> OpenSSLContextFactory::createTLSContext(const TLSOptions&, TLSContext::Mode mode) {
    return std::unique_ptr<TLSContext>(new OpenSSLContext(mode));
}

ByteArray OpenSSLContextFactory::convertDHParametersFromPEMToDER(const std::string& dhParametersInPEM) {
    ByteArray dhParametersInDER;

    auto bio = std::unique_ptr<BIO, decltype(&BIO_free)>(BIO_new(BIO_s_mem()), BIO_free);
    if (bio) {
        BIO_write(bio.get(), dhParametersInPEM.data(), dhParametersInPEM.size());
        if (auto params = PEM_read_bio_DHparams(bio.get(), nullptr, nullptr, nullptr)) {
            unsigned char* buffer = nullptr;
            auto len = i2d_DHparams(params, &buffer);
            if (len > 0) {
                dhParametersInDER = createByteArray(buffer, static_cast<size_t>(len));
                free(buffer);
            }
            DH_free(params);

        }
    }
    return dhParametersInDER;
}

void OpenSSLContextFactory::setCheckCertificateRevocation(bool check) {
    if (check) {
        SWIFT_LOG(warning) << "CRL Checking not supported for OpenSSL" << std::endl;
        assert(false);
    }
}

void OpenSSLContextFactory::setDisconnectOnCardRemoval(bool check) {
    if (check) {
        SWIFT_LOG(warning) << "Smart cards not supported for OpenSSL" << std::endl;
    }
}



}
