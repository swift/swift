/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>

#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Network/Timer.h>
#include <Swiften/Jingle/JingleSession.h>
#include <Swiften/Jingle/JingleContentID.h>
#include <Swiften/FileTransfer/IncomingFileTransfer.h>
#include <Swiften/FileTransfer/JingleTransport.h>
#include <Swiften/FileTransfer/JingleIncomingIBBTransport.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamClientSession.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServerSession.h>
#include <Swiften/Elements/JingleContentPayload.h>
#include <Swiften/Elements/JingleFileTransferDescription.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/Elements/S5BProxyRequest.h>
#include <Swiften/Elements/ErrorPayload.h>

namespace Swift {
	class IQRouter;
	class RemoteJingleTransportCandidateSelectorFactory;
	class LocalJingleTransportCandidateGeneratorFactory;
	class RemoteJingleTransportCandidateSelector;
	class LocalJingleTransportCandidateGenerator;
	class SOCKS5BytestreamRegistry;
	class SOCKS5BytestreamProxy;
	class IncrementalBytestreamHashCalculator;

	class IncomingJingleFileTransfer : public IncomingFileTransfer {
		public:
			typedef boost::shared_ptr<IncomingJingleFileTransfer> ref;
			enum State {
				Initial,
				CreatingInitialTransports,
				NegotiatingTransport,
				Transferring,
				WaitingForFallbackOrTerminate,
				Terminated
			};

			IncomingJingleFileTransfer(
					const JID& recipient,
					JingleSession::ref,
					JingleContentPayload::ref content,
					RemoteJingleTransportCandidateSelectorFactory*,
					LocalJingleTransportCandidateGeneratorFactory*,
					IQRouter* router,
					SOCKS5BytestreamRegistry* bytestreamRegistry,
					SOCKS5BytestreamProxy* bytestreamProxy,
					TimerFactory*);
			~IncomingJingleFileTransfer();

			virtual void accept(WriteBytestream::ref);
			virtual const JID& getSender() const;
			virtual const JID& getRecipient() const;
			void cancel();

		private:
			void handleSessionTerminateReceived(boost::optional<JinglePayload::Reason>);
			void handleSessionInfoReceived(JinglePayload::ref);
			void handleTransportReplaceReceived(const JingleContentID&, JingleTransportPayload::ref);
			void handleTransportInfoReceived(const JingleContentID&, JingleTransportPayload::ref);
			void handleLocalTransportCandidatesGenerated(JingleTransportPayload::ref candidates);
			void handleRemoteTransportCandidateSelectFinished(JingleTransportPayload::ref candidate);
			void setActiveTransport(JingleTransport::ref transport);
			void handleTransportDataReceived(const std::vector<unsigned char>& data);
			void handleWriteStreamDataReceived(const std::vector<unsigned char>& data);
			void stopActiveTransport();
			void checkCandidateSelected();
			JingleIncomingIBBTransport::ref createIBBTransport(JingleIBBTransportPayload::ref ibbTransport);
			JingleContentID getContentID() const;
			void checkIfAllDataReceived();
			bool verifyReceviedData();
			void finishOffTransfer();
			void handleTransferFinished(boost::optional<FileTransferError>);

		private:
			typedef std::map<std::string, JingleS5BTransportPayload::Candidate> CandidateMap;

		private:
			void activateProxySession(const JID &proxy);
			void handleActivateProxySessionResult(boost::shared_ptr<S5BProxyRequest> request, ErrorPayload::ref error);
			void proxySessionReady(const JID& proxy, bool error);

		private:
			void useOurCandidateChoiceForTransfer(JingleS5BTransportPayload::Candidate candidate);
			void useTheirCandidateChoiceForTransfer(JingleS5BTransportPayload::Candidate candidate);
			void decideOnUsedTransport();
			void fillCandidateMap(CandidateMap& map, JingleS5BTransportPayload::ref s5bPayload);

		private:
			JID ourJID;
			JingleSession::ref session;
			IQRouter* router;
			TimerFactory* timerFactory;
			JingleContentPayload::ref initialContent;
			State state;
			JingleFileTransferDescription::ref description;
			WriteBytestream::ref stream;
			boost::uintmax_t receivedBytes;
			IncrementalBytestreamHashCalculator* hashCalculator;
			Timer::ref waitOnHashTimer;
			IDGenerator idGenerator;

			RemoteJingleTransportCandidateSelector* candidateSelector;
			LocalJingleTransportCandidateGenerator* candidateGenerator;
			SOCKS5BytestreamRegistry* s5bRegistry;
			SOCKS5BytestreamProxy* s5bProxy;
			bool remoteTransportCandidateSelectFinished;
			JingleTransportPayload::ref selectedRemoteTransportCandidate;
			bool localTransportCandidateSelectFinished;
			JingleTransportPayload::ref selectedLocalTransportCandidate;

			JingleS5BTransportPayload::ref ourCandidate;
			JingleS5BTransportPayload::ref theirCandidate;
			CandidateMap ourCandidates;
			CandidateMap theirCandidates;
			SOCKS5BytestreamClientSession::ref clientSession;
			std::string s5bDestination;
			std::string s5bSessionID;
			SOCKS5BytestreamServerSession* serverSession;

			JingleTransport::ref activeTransport;
	};
}
