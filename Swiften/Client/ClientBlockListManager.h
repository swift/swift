/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Elements/BlockPayload.h>
#include <Swiften/Elements/BlockListPayload.h>
#include <Swiften/Elements/UnblockPayload.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Queries/SetResponder.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Client/BlockList.h>
#include <Swiften/Client/BlockListImpl.h>

namespace Swift {
	class IQRouter;

	class ClientBlockListManager {
		public:
			ClientBlockListManager(IQRouter *iqRouter);
			~ClientBlockListManager();

			/**
			 * Returns the blocklist.
			 */
			boost::shared_ptr<BlockList> getBlockList();

			GenericRequest<BlockPayload>::ref createBlockJIDRequest(const JID& jid);
			GenericRequest<BlockPayload>::ref createBlockJIDsRequest(const std::vector<JID>& jids);

			GenericRequest<UnblockPayload>::ref createUnblockJIDRequest(const JID& jid);
			GenericRequest<UnblockPayload>::ref createUnblockJIDsRequest(const std::vector<JID>& jids);
			GenericRequest<UnblockPayload>::ref createUnblockAllRequest();

		private:
			void handleBlockListReceived(boost::shared_ptr<BlockListPayload> payload, ErrorPayload::ref);

		private:
			IQRouter* iqRouter;
			boost::shared_ptr<GenericRequest<BlockListPayload> > getRequest;
			boost::shared_ptr<SetResponder<BlockPayload> > blockResponder;
			boost::shared_ptr<SetResponder<UnblockPayload> > unblockResponder;
			boost::shared_ptr<BlockListImpl> blockList;
	};
}

