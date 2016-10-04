/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/FileTransfer/WriteBytestream.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/IBB.h>
#include <Swiften/FileTransfer/FileTransferError.h>

namespace Swift {
	class IQRouter;

	class SWIFTEN_API IBBReceiveSession {
		public:
			IBBReceiveSession(
					const std::string& id, 
					const JID& from, 
					const JID& to,
					unsigned long long size, 
					boost::shared_ptr<WriteBytestream> bytestream, 
					IQRouter* router);
			~IBBReceiveSession();

			void start();
			void stop();

			const JID& getSender() const {
				return from;
			}

			const JID& getReceiver() const {
				return to;
			}

			boost::signal<void (boost::optional<FileTransferError>)> onFinished;

		private:
			bool handleSetRequest(const JID& from, const JID& to, const std::string& id, IBB::ref payload);
			void finish(boost::optional<FileTransferError>);

		private:
			class IBBResponder;
			friend class IBBResponder;

			std::string id;
			JID from;
			JID to;
			unsigned long long size;
			boost::shared_ptr<WriteBytestream> bytestream;
			IQRouter* router;
			IBBResponder* responder;
			bool active;
	};
}
