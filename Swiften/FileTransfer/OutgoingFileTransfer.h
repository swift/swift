/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/FileTransfer/ReadBytestream.h"
#include "Swiften/Base/boost_bsignals.h"
#include "Swiften/FileTransfer/FileTransferError.h"
#include "Swiften/FileTransfer/SOCKS5BytestreamServer.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/StreamInitiation.h"
#include "Swiften/Elements/Bytestreams.h"
#include "Swiften/Elements/ErrorPayload.h"
#include "Swiften/FileTransfer/IBBSendSession.h"

namespace Swift {
	class IQRouter;
	class SOCKS5BytestreamServer;

	class OutgoingFileTransfer {
		public:
			OutgoingFileTransfer(const String& id, const JID& from, const JID& to, const String& name, int size, const String& description, boost::shared_ptr<ReadBytestream> bytestream, IQRouter* iqRouter, SOCKS5BytestreamServer* socksServer);

			void start();
			void stop();

			boost::signal<void (const boost::optional<FileTransferError>&)> onFinished;

		private:
			void handleStreamInitiationRequestResponse(StreamInitiation::ref, const boost::optional<ErrorPayload>&);
			void handleBytestreamsRequestResponse(Bytestreams::ref, const boost::optional<ErrorPayload>&);
			void finish(boost::optional<FileTransferError> error);
			void handleIBBSessionFinished(boost::optional<FileTransferError> error);

		private:
			String id;
			JID from;
			JID to;
			String name;
			int size;
			String description;
			boost::shared_ptr<ReadBytestream> bytestream;
			IQRouter* iqRouter;
			SOCKS5BytestreamServer* socksServer;
			boost::shared_ptr<IBBSendSession> ibbSession;
	};
}
