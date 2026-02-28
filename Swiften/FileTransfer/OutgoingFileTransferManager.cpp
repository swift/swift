/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/OutgoingFileTransferManager.h>

#include <memory>

#include <Swiften/Base/IDGenerator.h>
#include <Swiften/FileTransfer/OutgoingJingleFileTransfer.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Jingle/JingleContentID.h>
#include <Swiften/Jingle/JingleSessionImpl.h>
#include <Swiften/Jingle/JingleSessionManager.h>

namespace Swift {

OutgoingFileTransferManager::OutgoingFileTransferManager(
        JingleSessionManager* jingleSessionManager,
        IQRouter* router,
        FileTransferTransporterFactory* transporterFactory,
        TimerFactory* timerFactory,
        CryptoProvider* crypto) :
            jingleSessionManager(jingleSessionManager),
            iqRouter(router),
            transporterFactory(transporterFactory),
            timerFactory(timerFactory),
            crypto(crypto) {
    idGenerator = new IDGenerator();
}

OutgoingFileTransferManager::~OutgoingFileTransferManager() {
    delete idGenerator;
}

std::shared_ptr<OutgoingFileTransfer> OutgoingFileTransferManager::createOutgoingFileTransfer(
        const JID& from,
        const JID& recipient,
        std::shared_ptr<ReadBytestream> readBytestream,
        const JingleFileTransferFileInfo& fileInfo,
        const FileTransferOptions& config) {
    JingleSessionImpl::ref jingleSession = std::make_shared<JingleSessionImpl>(
            from, recipient, idGenerator->generateID(), iqRouter);
    jingleSessionManager->registerOutgoingSession(from, jingleSession);
    return std::shared_ptr<OutgoingJingleFileTransfer>(new OutgoingJingleFileTransfer(
                recipient,
                jingleSession,
                readBytestream,
                transporterFactory,
                timerFactory,
                idGenerator,
                fileInfo,
                config,
                crypto));
}

}
