/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>

#include <Swiften/Elements/JingleTransportPayload.h>
#include <Swiften/FileTransfer/JingleTransport.h>

namespace Swift {
	class LocalJingleTransportCandidateGenerator {
		public:
			virtual ~LocalJingleTransportCandidateGenerator();
			/**
			* Should call onLocalTransportCandidatesGenerated if it has finished discovering local candidates.
			*/
			virtual void generateLocalTransportCandidates(JingleTransportPayload::ref) = 0;

			virtual bool isActualCandidate(JingleTransportPayload::ref) = 0;
			virtual int getPriority(JingleTransportPayload::ref) = 0;
			virtual JingleTransport::ref selectTransport(JingleTransportPayload::ref) = 0;

			boost::signal<void (JingleTransportPayload::ref)> onLocalTransportCandidatesGenerated;
	};
}
