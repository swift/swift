/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Client/ClientBlockListManager.h>

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <cassert>

#include <Swiften/Client/BlockListImpl.h>

using namespace Swift;

namespace {
	class BlockResponder : public SetResponder<BlockPayload> {
		public:
			BlockResponder(boost::shared_ptr<BlockListImpl> blockList, IQRouter* iqRouter) : SetResponder<BlockPayload>(iqRouter), blockList(blockList) {
			}

			virtual bool handleSetRequest(const JID& from, const JID&, const std::string& id, boost::shared_ptr<BlockPayload> payload) {
				if (getIQRouter()->isAccountJID(from)) {
						if (payload) {
							blockList->addItems(payload->getItems());
						}
						sendResponse(from, id, boost::shared_ptr<BlockPayload>());
				}
				else {
					sendError(from, id, ErrorPayload::NotAuthorized, ErrorPayload::Cancel);
				}
				return true;
			}

		private:
			boost::shared_ptr<BlockListImpl> blockList;
	};

	class UnblockResponder : public SetResponder<UnblockPayload> {
		public:
			UnblockResponder(boost::shared_ptr<BlockListImpl> blockList, IQRouter* iqRouter) : SetResponder<UnblockPayload>(iqRouter), blockList(blockList) {
			}

			virtual bool handleSetRequest(const JID& from, const JID&, const std::string& id, boost::shared_ptr<UnblockPayload> payload) {
				if (getIQRouter()->isAccountJID(from)) {
					if (payload) {
						if (payload->getItems().empty()) {
							blockList->removeAllItems();
						}
						else {
							blockList->removeItems(payload->getItems());
						}
					}
					sendResponse(from, id, boost::shared_ptr<UnblockPayload>());
				}
				else {
					sendError(from, id, ErrorPayload::NotAuthorized, ErrorPayload::Cancel);
				}
				return true;
			}

		private:
			boost::shared_ptr<BlockListImpl> blockList;
	};
}

ClientBlockListManager::ClientBlockListManager(IQRouter* iqRouter) : iqRouter(iqRouter) {
}

ClientBlockListManager::~ClientBlockListManager() {
	unblockResponder->stop();
	blockResponder->stop();
	if (getRequest) {
		getRequest->onResponse.disconnect(boost::bind(&ClientBlockListManager::handleBlockListReceived, this, _1, _2));
	}
}

boost::shared_ptr<BlockList> ClientBlockListManager::getBlockList() {
	if (!blockList) {
		blockList = boost::make_shared<BlockListImpl>();
		blockList->setState(BlockList::Requesting);
		assert(!getRequest);
		getRequest = boost::make_shared< GenericRequest<BlockListPayload> >(IQ::Get, JID(), boost::make_shared<BlockListPayload>(), iqRouter);
		getRequest->onResponse.connect(boost::bind(&ClientBlockListManager::handleBlockListReceived, this, _1, _2));
		getRequest->send();
	}
	return blockList;
}

void ClientBlockListManager::handleBlockListReceived(boost::shared_ptr<BlockListPayload> payload, ErrorPayload::ref error) {
	if (error || !payload) {
		blockList->setState(BlockList::Error);
	}
	else {
		blockList->setState(BlockList::Available);
		blockList->setItems(payload->getItems());
		blockResponder = boost::make_shared<BlockResponder>(blockList, iqRouter);
		blockResponder->start();
		unblockResponder = boost::make_shared<UnblockResponder>(blockList, iqRouter);
		unblockResponder->start();
	}
}

