/*
 * Copyright (c) 2011 Soren Dreijer
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "Swiften/TLS/Schannel/SchannelContextFactory.h"
#include "Swiften/TLS/Schannel/SchannelContext.h"

namespace Swift {

bool SchannelContextFactory::canCreate() const {
	return true;
}

TLSContext* SchannelContextFactory::createTLSContext() {
	return new SchannelContext();
}

}
