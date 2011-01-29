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

	class IncomingFileTransferManager : public IncomingJingleSessionHandler {
		public:
			IncomingFileTransferManager(JingleSessionManager* jingleSessionManager, IQRouter* router);
			~IncomingFileTransferManager();

			boost::signal<void (IncomingFileTransfer::ref)> onIncomingFileTransfer;

		private:
			bool handleIncomingJingleSession(IncomingJingleSession::ref session);

		private:
			JingleSessionManager* jingleSessionManager;
			IQRouter* router;
	};
}
