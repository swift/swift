/*
 * Copyright (c) 2013-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/FileTransfer/FileTransferTransporter.h>

namespace Swift {
	class LocalJingleTransportCandidateGenerator;
	class RemoteJingleTransportCandidateSelector;
	class SOCKS5BytestreamRegistry;
	class SOCKS5BytestreamServerManager;
	class SOCKS5BytestreamProxiesManager;
	class SOCKS5BytestreamClientSession;
	class SOCKS5BytestreamServerSession;
	class IDGenerator;
	class IQRouter;
	class ReadBytestream;
	class WriteBytestream;
	class ConnectionFactory;
	class TimerFactory;
	class CryptoProvider;
	class FileTransferOptions;

	class SWIFTEN_API DefaultFileTransferTransporter : public FileTransferTransporter {
		public:
			enum Role {
				Initiator,
				Responder
			};

			DefaultFileTransferTransporter(
				const JID& initiator, 
				const JID& responder,
				Role role,
				SOCKS5BytestreamRegistry*, 
				SOCKS5BytestreamServerManager* s5bServerManager,
				SOCKS5BytestreamProxiesManager* s5bProxy, 
				IDGenerator* idGenerator, 
				ConnectionFactory*, 
				TimerFactory*, 
				CryptoProvider*,
				IQRouter*,
				const FileTransferOptions&);
			virtual ~DefaultFileTransferTransporter();

			
			virtual void initialize();
			virtual void initialize(const std::string& s5bSessionID);

			virtual void startGeneratingLocalCandidates() SWIFTEN_OVERRIDE;
			virtual void stopGeneratingLocalCandidates() SWIFTEN_OVERRIDE;

			virtual void addRemoteCandidates(
					const std::vector<JingleS5BTransportPayload::Candidate>&) SWIFTEN_OVERRIDE;
			virtual void startTryingRemoteCandidates() SWIFTEN_OVERRIDE;
			virtual void stopTryingRemoteCandidates() SWIFTEN_OVERRIDE;

			virtual void startActivatingProxy(const JID& jid) SWIFTEN_OVERRIDE;
			virtual void stopActivatingProxy() SWIFTEN_OVERRIDE;

			virtual boost::shared_ptr<TransportSession> createIBBSendSession(
					const std::string& sessionID, unsigned int blockSize, boost::shared_ptr<ReadBytestream>) SWIFTEN_OVERRIDE;
			virtual boost::shared_ptr<TransportSession> createIBBReceiveSession(
					const std::string& sessionID, unsigned long long size, boost::shared_ptr<WriteBytestream>) SWIFTEN_OVERRIDE;
			virtual boost::shared_ptr<TransportSession> createRemoteCandidateSession(
					boost::shared_ptr<ReadBytestream>) SWIFTEN_OVERRIDE;
			virtual boost::shared_ptr<TransportSession> createRemoteCandidateSession(
					boost::shared_ptr<WriteBytestream>) SWIFTEN_OVERRIDE;
			virtual boost::shared_ptr<TransportSession> createLocalCandidateSession(
					boost::shared_ptr<ReadBytestream>) SWIFTEN_OVERRIDE;
			virtual boost::shared_ptr<TransportSession> createLocalCandidateSession(
					boost::shared_ptr<WriteBytestream>) SWIFTEN_OVERRIDE;

		private:
			void handleLocalCandidatesGenerated(const std::vector<JingleS5BTransportPayload::Candidate>&);
			void handleRemoteCandidateSelectFinished(
					const boost::optional<JingleS5BTransportPayload::Candidate>&, 
					boost::shared_ptr<SOCKS5BytestreamClientSession>);
			std::string getSOCKS5DstAddr() const;
			void closeLocalSession();
			void closeRemoteSession();
			boost::shared_ptr<SOCKS5BytestreamServerSession> getServerSession();

		private:
			JID initiator;
			JID responder;
			Role role;
			SOCKS5BytestreamRegistry* s5bRegistry;
			SOCKS5BytestreamServerManager* s5bServerManager;
			CryptoProvider* crypto;
			IQRouter* router;
			LocalJingleTransportCandidateGenerator* localCandidateGenerator;
			RemoteJingleTransportCandidateSelector* remoteCandidateSelector;
			std::string s5bSessionID;
			boost::shared_ptr<SOCKS5BytestreamClientSession> remoteS5BClientSession;
	};
}

