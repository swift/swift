/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/cstdint.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/FileTransfer/FileTransferOptions.h>
#include <Swiften/FileTransfer/IncomingFileTransfer.h>
#include <Swiften/FileTransfer/JingleFileTransfer.h>
#include <Swiften/Jingle/JingleContentID.h>

namespace Swift {
    class CryptoProvider;
    class FileTransferTransporterFactory;
    class IncrementalBytestreamHashCalculator;
    class JID;
    class JingleContentPayload;
    class JingleFileTransferDescription;
    class JingleSession;
    class Timer;
    class TimerFactory;

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
            virtual ~IncomingJingleFileTransfer() override;

            virtual void accept(std::shared_ptr<WriteBytestream>, const FileTransferOptions& = FileTransferOptions()) override;
            virtual void cancel() override;

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
                    boost::optional<JinglePayload::Reason> reason) override;
            virtual void handleSessionInfoReceived(std::shared_ptr<JinglePayload>) override;
            virtual void handleTransportReplaceReceived(
                    const JingleContentID&, std::shared_ptr<JingleTransportPayload>) override;

            virtual void handleLocalTransportCandidatesGenerated(
                    const std::string& s5bSessionID,
                    const std::vector<JingleS5BTransportPayload::Candidate>&,
                    const std::string& dstAddr) override;

            void handleWriteStreamDataReceived(const std::vector<unsigned char>& data);
            void stopActiveTransport();
            void checkCandidateSelected();
            virtual JingleContentID getContentID() const override;
            void checkIfAllDataReceived();
            bool verifyData();
            void handleWaitOnHashTimerTicked();
            void handleTransferFinished(boost::optional<FileTransferError>);

        private:
            virtual void startTransferViaRemoteCandidate() override;
            virtual void startTransferViaLocalCandidate() override;
            void checkHashAndTerminate();
            void stopAll();
            void setState(State state);
            void setFinishedState(FileTransfer::State::Type, const boost::optional<FileTransferError>& error);
            const JID& getSender() const override;
            const JID& getRecipient() const override;
            static FileTransfer::State::Type getExternalState(State state);
            virtual bool hasPriorityOnCandidateTie() const override;
            virtual void fallback() override;
            virtual void startTransferring(std::shared_ptr<TransportSession>) override;
            virtual bool isWaitingForPeerProxyActivate() const override;
            virtual bool isWaitingForLocalProxyActivate() const override;
            virtual bool isTryingCandidates() const override;
            virtual std::shared_ptr<TransportSession> createLocalCandidateSession() override;
            virtual std::shared_ptr<TransportSession> createRemoteCandidateSession() override;
            virtual void terminate(JinglePayload::Reason::Type reason) override;


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

            boost::signals2::scoped_connection writeStreamDataReceivedConnection;
            boost::signals2::scoped_connection waitOnHashTimerTickedConnection;
            boost::signals2::connection transferFinishedConnection;
    };
}
