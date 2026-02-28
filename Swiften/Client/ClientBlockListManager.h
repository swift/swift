/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Client/BlockList.h>
#include <Swiften/Client/BlockListImpl.h>
#include <Swiften/Elements/BlockListPayload.h>
#include <Swiften/Elements/BlockPayload.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/UnblockPayload.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Queries/SetResponder.h>

namespace Swift {
    class IQRouter;

    class SWIFTEN_API ClientBlockListManager {
        public:
            ClientBlockListManager(IQRouter *iqRouter);
            ~ClientBlockListManager();

            /**
             * Returns the blocklist.
             */
            std::shared_ptr<BlockList> getBlockList();

            /**
             * Get the blocklist from the server.
             */
            std::shared_ptr<BlockList> requestBlockList();

            GenericRequest<BlockPayload>::ref createBlockJIDRequest(const JID& jid);
            GenericRequest<BlockPayload>::ref createBlockJIDsRequest(const std::vector<JID>& jids);

            GenericRequest<UnblockPayload>::ref createUnblockJIDRequest(const JID& jid);
            GenericRequest<UnblockPayload>::ref createUnblockJIDsRequest(const std::vector<JID>& jids);
            GenericRequest<UnblockPayload>::ref createUnblockAllRequest();

        private:
            void handleBlockListReceived(std::shared_ptr<BlockListPayload> payload, ErrorPayload::ref);

        private:
            IQRouter* iqRouter;
            std::shared_ptr<SetResponder<BlockPayload> > blockResponder;
            std::shared_ptr<SetResponder<UnblockPayload> > unblockResponder;
            std::shared_ptr<BlockListImpl> blockList;
    };
}

