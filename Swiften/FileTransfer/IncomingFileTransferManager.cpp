/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/IncomingFileTransferManager.h>

#include <memory>

#include <Swiften/Base/Log.h>
#include <Swiften/Elements/JingleDescription.h>
#include <Swiften/Elements/JingleFileTransferDescription.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/FileTransfer/IncomingJingleFileTransfer.h>
#include <Swiften/Jingle/Jingle.h>
#include <Swiften/Jingle/JingleSessionManager.h>

namespace Swift {

IncomingFileTransferManager::IncomingFileTransferManager(
        JingleSessionManager* jingleSessionManager,
        FileTransferTransporterFactory* transporterFactory,
        TimerFactory* timerFactory,
        CryptoProvider* crypto) :
            jingleSessionManager(jingleSessionManager),
            transporterFactory(transporterFactory),
            timerFactory(timerFactory),
            crypto(crypto) {
    jingleSessionManager->addIncomingSessionHandler(this);
}

IncomingFileTransferManager::~IncomingFileTransferManager() {
    jingleSessionManager->removeIncomingSessionHandler(this);
}

bool IncomingFileTransferManager::handleIncomingJingleSession(
        JingleSession::ref session,
        const std::vector<JingleContentPayload::ref>& contents,
        const JID& recipient) {
    if (JingleContentPayload::ref content = Jingle::getContentWithDescription<JingleFileTransferDescription>(contents)) {
        if (content->getTransport<JingleS5BTransportPayload>() || content->getTransport<JingleIBBTransportPayload>()) {
            JingleFileTransferDescription::ref description = content->getDescription<JingleFileTransferDescription>();
            if (description) {
                IncomingJingleFileTransfer::ref transfer = std::make_shared<IncomingJingleFileTransfer>(
                        recipient, session, content, transporterFactory, timerFactory, crypto);
                onIncomingFileTransfer(transfer);
            }
            else {
                SWIFT_LOG(warning) << "Received a file-transfer request with no file description.";
                session->sendTerminate(JinglePayload::Reason::FailedApplication);
            }
        }
        else {
            session->sendTerminate(JinglePayload::Reason::UnsupportedTransports);
        }
        return true;
    }
    else {
        return false;
    }
}


}
