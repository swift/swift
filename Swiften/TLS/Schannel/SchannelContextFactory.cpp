/*
 * Copyright (c) 2011 Soren Dreijer
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include "Swiften/TLS/Schannel/SchannelContextFactory.h"
#include "Swiften/TLS/Schannel/SchannelContext.h"

namespace Swift {

SchannelContextFactory::SchannelContextFactory() : checkCertificateRevocation(true) {
}

bool SchannelContextFactory::canCreate() const {
	return true;
}

TLSContext* SchannelContextFactory::createTLSContext(const TLSOptions& tlsOptions) {
	SchannelContext* context = new SchannelContext(tlsOptions.schannelTLS1_0Workaround);
	context->setCheckCertificateRevocation(checkCertificateRevocation);
	return context;
}

void SchannelContextFactory::setCheckCertificateRevocation(bool b) {
	checkCertificateRevocation = b;
}


}
