/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>

#include <Swiften/JID/JID.h>
#include <Swiften/Elements/JingleTransportPayload.h>
#include <Swiften/FileTransfer/JingleTransport.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamClientSession.h>

namespace Swift {
	class RemoteJingleTransportCandidateSelector {
		public:
			virtual ~RemoteJingleTransportCandidateSelector();

			virtual void addRemoteTransportCandidates(JingleTransportPayload::ref) = 0;
			virtual void selectCandidate() = 0;
			virtual void setMinimumPriority(int) = 0;
			virtual void setRequesterTargtet(const JID&, const JID&) {}
			virtual SOCKS5BytestreamClientSession::ref getS5BSession() { return SOCKS5BytestreamClientSession::ref(); }

			virtual bool isActualCandidate(JingleTransportPayload::ref) = 0;
			virtual int getPriority(JingleTransportPayload::ref) = 0;
			virtual JingleTransport::ref selectTransport(JingleTransportPayload::ref) = 0;

			boost::signal<void (JingleTransportPayload::ref)> onRemoteTransportCandidateSelectFinished;
	};
}
