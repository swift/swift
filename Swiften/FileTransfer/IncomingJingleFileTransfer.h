/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>
#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Jingle/JingleContentID.h>
#include <Swiften/FileTransfer/IncomingFileTransfer.h>
#include <Swiften/FileTransfer/JingleFileTransfer.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/FileTransfer/FileTransferOptions.h>

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

	class SWIFTEN_API IncomingJingleFileTransfer : public IncomingFileTransfer, public JingleFileTransfer {
		public:
			typedef boost::shared_ptr<IncomingJingleFileTransfer> ref;

			IncomingJingleFileTransfer(
				const JID& recipient,
				boost::shared_ptr<JingleSession>,
				boost::shared_ptr<JingleContentPayload> content,
				FileTransferTransporterFactory*,
				TimerFactory*,
				CryptoProvider*);
			~IncomingJingleFileTransfer();

			virtual void accept(boost::shared_ptr<WriteBytestream>, const FileTransferOptions&) SWIFTEN_OVERRIDE;
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
			virtual void handleSessionInfoReceived(boost::shared_ptr<JinglePayload>) SWIFTEN_OVERRIDE;
			virtual void handleTransportReplaceReceived(
					const JingleContentID&, boost::shared_ptr<JingleTransportPayload>) SWIFTEN_OVERRIDE;

			virtual void handleLocalTransportCandidatesGenerated(
					const std::string& s5bSessionID, 
					const std::vector<JingleS5BTransportPayload::Candidate>&) SWIFTEN_OVERRIDE;

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
			virtual void startTransferring(boost::shared_ptr<TransportSession>) SWIFTEN_OVERRIDE;
			virtual bool isWaitingForPeerProxyActivate() const SWIFTEN_OVERRIDE;
			virtual bool isWaitingForLocalProxyActivate() const SWIFTEN_OVERRIDE;
			virtual bool isTryingCandidates() const SWIFTEN_OVERRIDE;
			virtual boost::shared_ptr<TransportSession> createLocalCandidateSession() SWIFTEN_OVERRIDE;
			virtual boost::shared_ptr<TransportSession> createRemoteCandidateSession() SWIFTEN_OVERRIDE;
			virtual void terminate(JinglePayload::Reason::Type reason) SWIFTEN_OVERRIDE;


		private:
			boost::shared_ptr<JingleContentPayload> initialContent;
			CryptoProvider* crypto;
			State state;
			boost::shared_ptr<JingleFileTransferDescription> description;
			boost::shared_ptr<WriteBytestream> stream;
			boost::uintmax_t receivedBytes;
			IncrementalBytestreamHashCalculator* hashCalculator;
			boost::shared_ptr<Timer> waitOnHashTimer;
			std::string hashAlgorithm;
			std::string hash;
			FileTransferOptions options;

			boost::bsignals::scoped_connection writeStreamDataReceivedConnection;
			boost::bsignals::scoped_connection waitOnHashTimerTickedConnection;
			boost::bsignals::connection transferFinishedConnection;
	};
}
