/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/FileTransfer/FileTransfer.h>
#include <Swiften/Jingle/AbstractJingleSessionListener.h>
#include <Swiften/Jingle/JingleContentID.h>

namespace Swift {
    class JingleSession;
    class FileTransferTransporter;
    class FileTransferTransporterFactory;
    class TransportSession;

    class SWIFTEN_API JingleFileTransfer : public AbstractJingleSessionListener {
        public:
            JingleFileTransfer(
                    std::shared_ptr<JingleSession>,
                    const JID& target,
                    FileTransferTransporterFactory*);
            virtual ~JingleFileTransfer();

        protected:
            virtual void handleTransportInfoReceived(const JingleContentID&, JingleTransportPayload::ref);
            virtual void handleLocalTransportCandidatesGenerated(
                    const std::string& s5bSessionID,
                    const std::vector<JingleS5BTransportPayload::Candidate>&,
                    const std::string& dstAddr) = 0;
            virtual void handleProxyActivateFinished(
                    const std::string& s5bSessionID,
                    ErrorPayload::ref error);
            virtual void decideOnCandidates();
            void handleRemoteTransportCandidateSelectFinished(
                    const std::string& s5bSessionID, const boost::optional<JingleS5BTransportPayload::Candidate>&);
            virtual JingleContentID getContentID() const = 0;
            virtual void startTransferring(std::shared_ptr<TransportSession>) = 0;
            virtual void terminate(JinglePayload::Reason::Type reason) = 0;
            virtual void fallback() = 0;
            virtual bool hasPriorityOnCandidateTie() const = 0;
            virtual bool isWaitingForPeerProxyActivate() const = 0;
            virtual bool isWaitingForLocalProxyActivate() const = 0;
            virtual bool isTryingCandidates() const = 0;
            virtual std::shared_ptr<TransportSession> createLocalCandidateSession() = 0;
            virtual std::shared_ptr<TransportSession> createRemoteCandidateSession() = 0;
            virtual void startTransferViaLocalCandidate() = 0;
            virtual void startTransferViaRemoteCandidate() = 0;

        protected:
            typedef std::map<std::string, JingleS5BTransportPayload::Candidate> CandidateMap;

            void setTransporter(FileTransferTransporter* transporter);
            void removeTransporter();
            void fillCandidateMap(
                    CandidateMap& map,
                    const std::vector<JingleS5BTransportPayload::Candidate>&);
            const JID& getInitiator() const;
            const JID& getResponder() const;

            static FileTransfer::State::Type getExternalFinishedState(JinglePayload::Reason::Type);
            static boost::optional<FileTransferError> getFileTransferError(JinglePayload::Reason::Type);

            std::shared_ptr<JingleSession> session;
            JID target;
            FileTransferTransporterFactory* transporterFactory;
            FileTransferTransporter* transporter;

            std::string candidateSelectRequestID;
            bool ourCandidateSelectFinished;
            boost::optional<JingleS5BTransportPayload::Candidate> ourCandidateChoice;
            bool theirCandidateSelectFinished;
            boost::optional<JingleS5BTransportPayload::Candidate> theirCandidateChoice;
            CandidateMap localCandidates;

            std::shared_ptr<TransportSession> transportSession;

            boost::signals2::scoped_connection localTransportCandidatesGeneratedConnection;
            boost::signals2::scoped_connection remoteTransportCandidateSelectFinishedConnection;
            boost::signals2::scoped_connection proxyActivatedConnection;
    };
}
