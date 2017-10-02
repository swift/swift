/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/TLS/PlatformTLSFactories.h>

#include <Swiften/Base/Platform.h>
#include <Swiften/TLS/CertificateFactory.h>
#include <Swiften/TLS/TLSContextFactory.h>
#ifdef HAVE_OPENSSL
    #include <Swiften/TLS/OpenSSL/OpenSSLContextFactory.h>
    #include <Swiften/TLS/OpenSSL/OpenSSLCertificateFactory.h>
#endif
#ifdef HAVE_SCHANNEL
    #include <Swiften/TLS/Schannel/SchannelContextFactory.h>
    #include <Swiften/TLS/Schannel/SchannelCertificateFactory.h>
#endif
#ifdef HAVE_SECURETRANSPORT
    #include <Swiften/TLS/SecureTransport/SecureTransportContextFactory.h>
    #include <Swiften/TLS/SecureTransport/SecureTransportCertificateFactory.h>
#endif

namespace Swift {

PlatformTLSFactories::PlatformTLSFactories() : contextFactory(nullptr), certificateFactory(nullptr) {
#ifdef HAVE_OPENSSL
    contextFactory = new OpenSSLContextFactory();
    certificateFactory = new OpenSSLCertificateFactory();
#endif
#ifdef HAVE_SCHANNEL
    contextFactory = new SchannelContextFactory();
    certificateFactory = new SchannelCertificateFactory();
#endif
#ifdef HAVE_SECURETRANSPORT
    contextFactory = new SecureTransportContextFactory();
    certificateFactory = new SecureTransportCertificateFactory();
#endif
}

PlatformTLSFactories::~PlatformTLSFactories() {
    delete contextFactory;
    delete certificateFactory;
}

TLSContextFactory* PlatformTLSFactories::getTLSContextFactory() const {
    return contextFactory;
}

CertificateFactory* PlatformTLSFactories::getCertificateFactory() const {
    return certificateFactory;
}

}
