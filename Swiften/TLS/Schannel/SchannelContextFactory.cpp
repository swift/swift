/*
 * Copyright (c) 2011 Soren Dreijer
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/TLS/Schannel/SchannelContextFactory.h>

#include <Swiften/TLS/Schannel/SchannelContext.h>

namespace Swift {

SchannelContextFactory::SchannelContextFactory() : checkCertificateRevocation(true), disconnectOnCardRemoval(true) {
}

bool SchannelContextFactory::canCreate() const {
    return true;
}

std::unique_ptr<TLSContext> SchannelContextFactory::createTLSContext(const TLSOptions& tlsOptions, TLSContext::Mode mode) {
    // TLS server mode is not supported for the SecureTransport backend yet.
    assert(mode == TLSContext::Mode::Client);
    SchannelContext* context = new SchannelContext(tlsOptions.schannelTLS1_0Workaround);
    context->setCheckCertificateRevocation(checkCertificateRevocation);
    context->setDisconnectOnCardRemoval(disconnectOnCardRemoval);
    return std::unique_ptr<TLSContext>(context);
}

void SchannelContextFactory::setCheckCertificateRevocation(bool b) {
    checkCertificateRevocation = b;
}

void SchannelContextFactory::setDisconnectOnCardRemoval(bool b) {
    disconnectOnCardRemoval = b;
}

}
