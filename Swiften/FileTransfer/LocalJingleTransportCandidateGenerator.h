/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2013-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/FileTransfer/LocalJingleTransportCandidateGenerator.h>

#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Base/Override.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/FileTransfer/FileTransferOptions.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamProxyFinder.h>

namespace Swift {
	class SOCKS5BytestreamServerManager;
	class SOCKS5BytestreamProxiesManager;
	class SOCKS5BytestreamServerInitializeRequest;
	class SOCKS5BytestreamServerResourceUser;
	class SOCKS5BytestreamServerPortForwardingUser;
	class JingleS5BTransportPayload;

	class LocalJingleTransportCandidateGenerator {
		public:
			LocalJingleTransportCandidateGenerator(
					SOCKS5BytestreamServerManager* s5bServerManager,
					SOCKS5BytestreamProxiesManager* s5bProxy, 
					const JID& ownJID,
					IDGenerator* idGenerator,
					const FileTransferOptions& options);
			virtual ~LocalJingleTransportCandidateGenerator();

			virtual void start();
			virtual void stop();

			boost::signal<void (const std::vector<JingleS5BTransportPayload::Candidate>&)> onLocalTransportCandidatesGenerated;

		private:
			void handleS5BServerInitialized(bool success);
			void handlePortForwardingSetup(bool success);
			void handleDiscoveredProxiesChanged();

			void checkS5BCandidatesReady();
			void emitOnLocalTransportCandidatesGenerated();

		private:
			SOCKS5BytestreamServerManager* s5bServerManager;
			SOCKS5BytestreamProxiesManager* s5bProxy;
			JID ownJID;
			IDGenerator* idGenerator;
			boost::shared_ptr<SOCKS5BytestreamServerInitializeRequest> s5bServerInitializeRequest;
			boost::shared_ptr<SOCKS5BytestreamServerResourceUser> s5bServerResourceUser_;
			boost::shared_ptr<SOCKS5BytestreamServerPortForwardingUser> s5bServerPortForwardingUser_;
			bool triedServerInit_;
			bool triedForwarding_;
			bool triedProxyDiscovery_;
			FileTransferOptions options_;
	};
}
