/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/FailingTransportSession.h>

#include <Swiften/Base/Log.h>

namespace Swift {

FailingTransportSession::~FailingTransportSession() {
}

void FailingTransportSession::start() {
    SWIFT_LOG(error) << "Trying to start failing transport." << std::endl;
    onFinished(FileTransferError(FileTransferError::PeerError));
}

void FailingTransportSession::stop() {
}

}
