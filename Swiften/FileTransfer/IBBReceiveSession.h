/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional/optional_fwd.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/FileTransfer/WriteBytestream.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/IBB.h>
#include <Swiften/FileTransfer/FileTransferError.h>

namespace Swift {
	class IQRouter;

	class IBBReceiveSession {
		public:
			IBBReceiveSession(
					const std::string& id, 
					const JID& from, 
					const JID& to,
					size_t size, 
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

			boost::signal<void (const std::vector<unsigned char>&)> onDataReceived;
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
			size_t size;
			IQRouter* router;
			IBBResponder* responder;
			bool active;
	};
}
