/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/cstdint.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/FileTransfer/FileTransferOptions.h>
#include <Swiften/FileTransfer/IncomingFileTransfer.h>
#include <Swiften/FileTransfer/JingleFileTransfer.h>
#include <Swiften/Jingle/JingleContentID.h>

namespace Swift {
    class JID;
    class JingleSession;
    class JingleContentPayload;
    class FileTransferTransporter;
    class FileTransferTransporterFactory;
    class TimerFactory;
    class Timer;
    class CryptoProvider;
    class IncrementalBytestreamHashCalculator;
    class JingleFileTransferDescription;
    class HashElement;

    /**
     * @brief The IncomingJingleFileTransfer class contains the business logic for managing incoming
     *        Jingle file transfers.
     *
     * Calling IncomingJingleFileTransfer::accept will start to negotiate possible transfer
     * methods and after a working method has been decided among peers the trasnfer is started.
     */
    class SWIFTEN_API IncomingJingleFileTransfer : public IncomingFileTransfer, public JingleFileTransfer {
        public:
            typedef std::shared_ptr<IncomingJingleFileTransfer> ref;

            IncomingJingleFileTransfer(
                const JID& recipient,
                std::shared_ptr<JingleSession>,
                std::shared_ptr<JingleContentPayload> content,
                FileTransferTransporterFactory*,
                TimerFactory*,
                CryptoProvider*);
            virtual ~IncomingJingleFileTransfer();

            virtual void accept(std::shared_ptr<WriteBytestream>, const FileTransferOptions& = FileTransferOptions()) SWIFTEN_OVERRIDE;
            virtual void cancel() SWIFTEN_OVERRIDE;

        private:
            enum State {
                Initial,
                GeneratingInitialLocalCandidates,
                TryingCandidates,
                WaitingForPeerProxyActivate,
                WaitingForLocalProxyActivate,
                WaitingForFallbackOrTerminate,
                Transferring,
                WaitingForHash,
                Finished
            };

            virtual void handleSessionTerminateReceived(
                    boost::optional<JinglePayload::Reason> reason) SWIFTEN_OVERRIDE;
            virtual void handleSessionInfoReceived(std::shared_ptr<JinglePayload>) SWIFTEN_OVERRIDE;
            virtual void handleTransportReplaceReceived(
                    const JingleContentID&, std::shared_ptr<JingleTransportPayload>) SWIFTEN_OVERRIDE;

            virtual void handleLocalTransportCandidatesGenerated(
                    const std::string& s5bSessionID,
                    const std::vector<JingleS5BTransportPayload::Candidate>&,
                    const std::string& dstAddr) SWIFTEN_OVERRIDE;

            void handleWriteStreamDataReceived(const std::vector<unsigned char>& data);
            void stopActiveTransport();
            void checkCandidateSelected();
            virtual JingleContentID getContentID() const SWIFTEN_OVERRIDE;
            void checkIfAllDataReceived();
            bool verifyData();
            void handleWaitOnHashTimerTicked();
            void handleTransferFinished(boost::optional<FileTransferError>);

        private:
            virtual void startTransferViaRemoteCandidate() SWIFTEN_OVERRIDE;
            virtual void startTransferViaLocalCandidate() SWIFTEN_OVERRIDE;
            void checkHashAndTerminate();
            void stopAll();
            void setState(State state);
            void setFinishedState(FileTransfer::State::Type, const boost::optional<FileTransferError>& error);
            const JID& getSender() const SWIFTEN_OVERRIDE;
            const JID& getRecipient() const SWIFTEN_OVERRIDE;
            static FileTransfer::State::Type getExternalState(State state);
            virtual bool hasPriorityOnCandidateTie() const SWIFTEN_OVERRIDE;
            virtual void fallback() SWIFTEN_OVERRIDE;
            virtual void startTransferring(std::shared_ptr<TransportSession>) SWIFTEN_OVERRIDE;
            virtual bool isWaitingForPeerProxyActivate() const SWIFTEN_OVERRIDE;
            virtual bool isWaitingForLocalProxyActivate() const SWIFTEN_OVERRIDE;
            virtual bool isTryingCandidates() const SWIFTEN_OVERRIDE;
            virtual std::shared_ptr<TransportSession> createLocalCandidateSession() SWIFTEN_OVERRIDE;
            virtual std::shared_ptr<TransportSession> createRemoteCandidateSession() SWIFTEN_OVERRIDE;
            virtual void terminate(JinglePayload::Reason::Type reason) SWIFTEN_OVERRIDE;


        private:
            std::shared_ptr<JingleContentPayload> initialContent;
            CryptoProvider* crypto;
            State state;
            std::shared_ptr<JingleFileTransferDescription> description;
            std::shared_ptr<WriteBytestream> stream;
            boost::uintmax_t receivedBytes;
            IncrementalBytestreamHashCalculator* hashCalculator;
            std::shared_ptr<Timer> waitOnHashTimer;
            std::map<std::string, ByteArray> hashes;
            FileTransferOptions options;

            boost::bsignals::scoped_connection writeStreamDataReceivedConnection;
            boost::bsignals::scoped_connection waitOnHashTimerTickedConnection;
            boost::bsignals::connection transferFinishedConnection;
    };
}
