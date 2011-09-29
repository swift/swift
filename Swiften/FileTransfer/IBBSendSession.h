/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/FileTransfer/ReadBytestream.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/IBB.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/FileTransfer/FileTransferError.h>

namespace Swift {
	class IQRouter;

	class IBBSendSession {
		public:
			IBBSendSession(const std::string& id, const JID& from, const JID& to, boost::shared_ptr<ReadBytestream> bytestream, IQRouter* router);
			~IBBSendSession();

			void start();
			void stop();

			const JID& getSender() const {
				return from;
			}

			const JID& getReceiver() const {
				return to;
			}

			void setBlockSize(int blockSize) {
				this->blockSize = blockSize;
			}

			boost::signal<void (boost::optional<FileTransferError>)> onFinished;
			boost::signal<void (int)> onBytesSent;

		private:
			void handleIBBResponse(IBB::ref, ErrorPayload::ref);
			void finish(boost::optional<FileTransferError>);
			void sendMoreData();
			void handleDataAvailable();

		private:
			std::string id;
			JID from;
			JID to;
			boost::shared_ptr<ReadBytestream> bytestream;
			IQRouter* router;
			int blockSize;
			int sequenceNumber;
			bool active;
			bool waitingForData;
	};
}
