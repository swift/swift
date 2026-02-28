/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/FileTransfer/IncomingFileTransfer.h>
#include <Swiften/Jingle/IncomingJingleSessionHandler.h>

namespace Swift {
    class JingleSessionManager;
    class FileTransferTransporterFactory;
    class TimerFactory;
    class CryptoProvider;

    class SWIFTEN_API IncomingFileTransferManager : public IncomingJingleSessionHandler {
        public:
            IncomingFileTransferManager(
                    JingleSessionManager* jingleSessionManager,
                    FileTransferTransporterFactory* transporterFactory,
                    TimerFactory* timerFactory,
                    CryptoProvider* crypto);
            virtual ~IncomingFileTransferManager();

            boost::signals2::signal<void (IncomingFileTransfer::ref)> onIncomingFileTransfer;

        private:
            bool handleIncomingJingleSession(
                    JingleSession::ref session,
                    const std::vector<JingleContentPayload::ref>& contents,
                    const JID& recipient);

        private:
            JingleSessionManager* jingleSessionManager;
            FileTransferTransporterFactory* transporterFactory;
            TimerFactory* timerFactory;
            CryptoProvider* crypto;
    };
}
