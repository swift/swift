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

#pragma once

#include <memory>

#include <Swiften/Base/API.h>

namespace Swift {
    class JingleSessionManager;
    class IQRouter;
    class FileTransferTransporterFactory;
    class OutgoingFileTransfer;
    class JID;
    class IDGenerator;
    class ReadBytestream;
    class JingleFileTransferFileInfo;
    class CryptoProvider;
    class FileTransferOptions;
    class TimerFactory;

    class SWIFTEN_API OutgoingFileTransferManager {
        public:
            OutgoingFileTransferManager(
                    JingleSessionManager* jingleSessionManager,
                    IQRouter* router,
                    FileTransferTransporterFactory* transporterFactory,
                    TimerFactory* timerFactory,
                    CryptoProvider* crypto);
            ~OutgoingFileTransferManager();

            std::shared_ptr<OutgoingFileTransfer> createOutgoingFileTransfer(
                    const JID& from,
                    const JID& to,
                    std::shared_ptr<ReadBytestream>,
                    const JingleFileTransferFileInfo&,
                    const FileTransferOptions&);

        private:
            JingleSessionManager* jingleSessionManager;
            IQRouter* iqRouter;
            FileTransferTransporterFactory* transporterFactory;
            TimerFactory* timerFactory;
            IDGenerator* idGenerator;
            CryptoProvider* crypto;
    };
}
