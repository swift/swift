/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "PlatformNATTraversalRequest.h"

#include <Swiften/Network/PlatformNATTraversalWorker.h>

namespace Swift {

PlatformNATTraversalRequest::PlatformNATTraversalRequest(PlatformNATTraversalWorker* worker) : worker(worker) {

}

PlatformNATTraversalRequest::~PlatformNATTraversalRequest() {

}

void PlatformNATTraversalRequest::doRun() {
	worker->addRequestToQueue(shared_from_this());
}

}
