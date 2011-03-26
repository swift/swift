/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Jingle/JingleSession.h>
#include <Swiften/Jingle/JingleContentID.h>
#include <Swiften/FileTransfer/IncomingFileTransfer.h>
#include <Swiften/FileTransfer/JingleTransport.h>
#include <Swiften/FileTransfer/JingleIncomingIBBTransport.h>
#include <Swiften/Elements/JingleContentPayload.h>
#include <Swiften/Elements/JingleFileTransferDescription.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>

namespace Swift {
	class IQRouter;
	class RemoteJingleTransportCandidateSelectorFactory;
	class LocalJingleTransportCandidateGeneratorFactory;
	class RemoteJingleTransportCandidateSelector;
	class LocalJingleTransportCandidateGenerator;

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
					JingleSession::ref,
					JingleContentPayload::ref content,
					RemoteJingleTransportCandidateSelectorFactory*,
					LocalJingleTransportCandidateGeneratorFactory*,
					IQRouter* router);
			~IncomingJingleFileTransfer();

			virtual void accept(WriteBytestream::ref);

		private:
			void handleSessionTerminateReceived();
			void handleTransportReplaceReceived(const JingleContentID&, JingleTransportPayload::ref);
			void handleTransportInfoReceived(const JingleContentID&, JingleTransportPayload::ref);
			void handleLocalTransportCandidatesGenerated(JingleTransportPayload::ref candidates);
			void handleRemoteTransportCandidateSelectFinished(JingleTransportPayload::ref candidate);
			void setActiveTransport(JingleTransport::ref transport);
			void handleTransportDataReceived(const std::vector<unsigned char>& data);
			void stopActiveTransport();
			void checkCandidateSelected();
			JingleIncomingIBBTransport::ref createIBBTransport(JingleIBBTransportPayload::ref ibbTransport);

		private:
			JingleSession::ref session;
			IQRouter* router;
			JingleContentPayload::ref initialContent;
			JingleContentID contentID;
			State state;
			JingleFileTransferDescription::ref description;
			WriteBytestream::ref stream;
			RemoteJingleTransportCandidateSelector* candidateSelector;
			LocalJingleTransportCandidateGenerator* candidateGenerator;
			bool remoteTransportCandidateSelectFinished;
			JingleTransportPayload::ref selectedRemoteTransportCandidate;
			bool localTransportCandidateSelectFinished;
			JingleTransportPayload::ref selectedLocalTransportCandidate;

			JingleTransport::ref activeTransport;
	};
}
