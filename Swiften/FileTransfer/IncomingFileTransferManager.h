/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/FileTransfer/IncomingFileTransfer.h>
#include <Swiften/Jingle/IncomingJingleSessionHandler.h>

namespace Swift {
	class IQRouter;
	class JingleSessionManager;
	class RemoteJingleTransportCandidateSelectorFactory;
	class LocalJingleTransportCandidateGeneratorFactory;
	class SOCKS5BytestreamRegistry;
	class SOCKS5BytestreamProxy;
	class TimerFactory;

	class IncomingFileTransferManager : public IncomingJingleSessionHandler {
		public:
			IncomingFileTransferManager(JingleSessionManager* jingleSessionManager, IQRouter* router, RemoteJingleTransportCandidateSelectorFactory* remoteFactory, LocalJingleTransportCandidateGeneratorFactory* localFactory, SOCKS5BytestreamRegistry* bytestreamRegistry, SOCKS5BytestreamProxy* bytestreamProxy, TimerFactory* timerFactory);
			~IncomingFileTransferManager();

			boost::signal<void (IncomingFileTransfer::ref)> onIncomingFileTransfer;

		private:
			bool handleIncomingJingleSession(JingleSession::ref session, const std::vector<JingleContentPayload::ref>& contents, const JID& recipient);

		private:
			JingleSessionManager* jingleSessionManager;
			IQRouter* router;
			RemoteJingleTransportCandidateSelectorFactory* remoteFactory;
			LocalJingleTransportCandidateGeneratorFactory* localFactory;
			SOCKS5BytestreamRegistry* bytestreamRegistry;
			SOCKS5BytestreamProxy* bytestreamProxy;
			TimerFactory* timerFactory;
	};
}
