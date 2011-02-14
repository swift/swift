/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include "Swiften/Base/boost_bsignals.h"
#include "Swiften/FileTransfer/WriteBytestream.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/IBB.h"
#include "Swiften/Elements/ErrorPayload.h"
#include "Swiften/FileTransfer/FileTransferError.h"
#include "Swiften/Queries/SetResponder.h"

namespace Swift {
	class IQRouter;

	class IBBReceiveSession : public SetResponder<IBB> {
		public:
			IBBReceiveSession(const std::string& id, const JID& from, size_t size, WriteBytestream::ref bytestream, IQRouter* router);
			~IBBReceiveSession();

			void start();
			void stop();

			boost::signal<void (boost::optional<FileTransferError>)> onFinished;

		private:
			bool handleSetRequest(const JID& from, const JID& to, const std::string& id, IBB::ref payload);
			void finish(boost::optional<FileTransferError>);

		private:
			std::string id;
			JID from;
			size_t size;
			WriteBytestream::ref bytestream;
			IQRouter* router;
			int sequenceNumber;
			bool active;
			size_t receivedSize;
	};
}
