/*
 * Copyright (c) 2016-2019 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/FailingTransportSession.h>

#include <Swiften/Base/Log.h>

namespace Swift {

FailingTransportSession::~FailingTransportSession() {
}

void FailingTransportSession::start() {
    SWIFT_LOG(error) << "Trying to start failing transport.";
    onFinished(FileTransferError(FileTransferError::PeerError));
}

void FailingTransportSession::stop() {
}

}
