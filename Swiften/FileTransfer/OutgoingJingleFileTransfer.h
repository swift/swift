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

#include <boost/optional/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/JingleFileTransferFileInfo.h>
#include <Swiften/FileTransfer/FileTransferOptions.h>
#include <Swiften/FileTransfer/JingleFileTransfer.h>
#include <Swiften/FileTransfer/OutgoingFileTransfer.h>
#include <Swiften/Jingle/JingleContentID.h>
#include <Swiften/Network/Timer.h>

namespace Swift {
    class CryptoProvider;
    class FileTransferTransporter;
    class FileTransferTransporterFactory;
    class IDGenerator;
    class IncrementalBytestreamHashCalculator;
    class ReadBytestream;
    class TimerFactory;
    class TransportSession;

    class SWIFTEN_API OutgoingJingleFileTransfer : public OutgoingFileTransfer, public JingleFileTransfer {
        public:
            OutgoingJingleFileTransfer(
                const JID& to,
                std::shared_ptr<JingleSession>,
                std::shared_ptr<ReadBytestream>,
                FileTransferTransporterFactory*,
                TimerFactory*,
                IDGenerator*,
                const JingleFileTransferFileInfo&,
                const FileTransferOptions&,
                CryptoProvider*);
            virtual ~OutgoingJingleFileTransfer();

            virtual void start() SWIFTEN_OVERRIDE;
            virtual void cancel() SWIFTEN_OVERRIDE;

        private:
            enum State {
                Initial,
                GeneratingInitialLocalCandidates,
                WaitingForAccept,
                TryingCandidates,
                WaitingForPeerProxyActivate,
                WaitingForLocalProxyActivate,
                WaitingForCandidateAcknowledge,
                FallbackRequested,
                Transferring,
                WaitForTermination,
                Finished
            };

            virtual void handleSessionAcceptReceived(const JingleContentID&, std::shared_ptr<JingleDescription>, std::shared_ptr<JingleTransportPayload>) SWIFTEN_OVERRIDE;
            virtual void handleSessionTerminateReceived(boost::optional<JinglePayload::Reason> reason) SWIFTEN_OVERRIDE;
            virtual void handleTransportAcceptReceived(const JingleContentID&, std::shared_ptr<JingleTransportPayload>) SWIFTEN_OVERRIDE;
            virtual void handleTransportRejectReceived(const JingleContentID &, std::shared_ptr<JingleTransportPayload>) SWIFTEN_OVERRIDE;
            virtual void startTransferViaRemoteCandidate() SWIFTEN_OVERRIDE;
            virtual void startTransferViaLocalCandidate() SWIFTEN_OVERRIDE;
            void startTransferringIfCandidateAcknowledged();

            virtual void handleLocalTransportCandidatesGenerated(const std::string& s5bSessionID, const std::vector<JingleS5BTransportPayload::Candidate>&, const std::string& dstAddr) SWIFTEN_OVERRIDE;
            virtual void handleTransportInfoAcknowledged(const std::string& id) SWIFTEN_OVERRIDE;

            virtual JingleContentID getContentID() const SWIFTEN_OVERRIDE;

            virtual void terminate(JinglePayload::Reason::Type reason) SWIFTEN_OVERRIDE;

            virtual void fallback() SWIFTEN_OVERRIDE;
            void handleTransferFinished(boost::optional<FileTransferError>);

            void sendSessionInfoHash();

            virtual void startTransferring(std::shared_ptr<TransportSession>) SWIFTEN_OVERRIDE;

            virtual bool hasPriorityOnCandidateTie() const SWIFTEN_OVERRIDE;
            virtual bool isWaitingForPeerProxyActivate() const SWIFTEN_OVERRIDE;
            virtual bool isWaitingForLocalProxyActivate() const SWIFTEN_OVERRIDE;
            virtual bool isTryingCandidates() const SWIFTEN_OVERRIDE;
            virtual std::shared_ptr<TransportSession> createLocalCandidateSession() SWIFTEN_OVERRIDE;
            virtual std::shared_ptr<TransportSession> createRemoteCandidateSession() SWIFTEN_OVERRIDE;

            void handleWaitForRemoteTerminationTimeout();

            void stopAll();
            void setInternalState(State state);
            void setFinishedState(FileTransfer::State::Type, const boost::optional<FileTransferError>& error);

            static FileTransfer::State::Type getExternalState(State state);

        private:
            IDGenerator* idGenerator;
            std::shared_ptr<ReadBytestream> stream;
            JingleFileTransferFileInfo fileInfo;
            FileTransferOptions options;
            JingleContentID contentID;
            IncrementalBytestreamHashCalculator* hashCalculator;
            State state;
            bool candidateAcknowledged;

            Timer::ref waitForRemoteTermination;

            boost::bsignals::connection processedBytesConnection;
            boost::bsignals::connection transferFinishedConnection;
    };

}
