/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <algorithm>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Client/ClientBlockListManager.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Queries/IQRouter.h>

using namespace Swift;

class ClientBlockListManagerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(ClientBlockListManagerTest);
        CPPUNIT_TEST(testFetchBlockList);
        CPPUNIT_TEST(testBlockCommand);
        CPPUNIT_TEST(testUnblockCommand);
        CPPUNIT_TEST(testUnblockAllCommand);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            ownJID_ = JID("kev@wonderland.lit");
            stanzaChannel_ = new DummyStanzaChannel();
            iqRouter_ = new IQRouter(stanzaChannel_);
            iqRouter_->setJID(ownJID_);
            clientBlockListManager_ = new ClientBlockListManager(iqRouter_);
        }

        void testFetchBlockList() {
            std::vector<JID> blockJids;
            blockJids.push_back(JID("romeo@montague.net"));
            blockJids.push_back(JID("iago@shakespeare.lit"));
            helperInitialBlockListFetch(blockJids);

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), clientBlockListManager_->getBlockList()->getItems().size());
        }

        void testBlockCommand() {
            // start with an already fetched block list
            helperInitialBlockListFetch(std::vector<JID>(1, JID("iago@shakespeare.lit")));

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), clientBlockListManager_->getBlockList()->getItems().size());
            CPPUNIT_ASSERT_EQUAL(BlockList::Available, clientBlockListManager_->getBlockList()->getState());

            GenericRequest<BlockPayload>::ref blockRequest = clientBlockListManager_->createBlockJIDRequest(JID("romeo@montague.net"));
            blockRequest->send();
            IQ::ref request = stanzaChannel_->getStanzaAtIndex<IQ>(2);
            CPPUNIT_ASSERT(request.get() != nullptr);
            std::shared_ptr<BlockPayload> blockPayload = request->getPayload<BlockPayload>();
            CPPUNIT_ASSERT(blockPayload.get() != nullptr);
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.net"), blockPayload->getItems().at(0));

            IQ::ref blockRequestResponse = IQ::createResult(request->getFrom(), JID(), request->getID());
            stanzaChannel_->sendIQ(blockRequestResponse);
            stanzaChannel_->onIQReceived(blockRequestResponse);

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), clientBlockListManager_->getBlockList()->getItems().size());

            // send block push
            std::shared_ptr<BlockPayload> pushPayload = std::make_shared<BlockPayload>();
            pushPayload->addItem(JID("romeo@montague.net"));
            IQ::ref blockPush = IQ::createRequest(IQ::Set, ownJID_, "push1", pushPayload);
            stanzaChannel_->sendIQ(blockPush);
            stanzaChannel_->onIQReceived(blockPush);

            std::vector<JID> blockedJIDs = clientBlockListManager_->getBlockList()->getItems();
            CPPUNIT_ASSERT(blockedJIDs.end() != std::find(blockedJIDs.begin(), blockedJIDs.end(), JID("romeo@montague.net")));
        }

        void testUnblockCommand() {
            // start with an already fetched block list
            std::vector<JID> initialBlockList = std::vector<JID>(1, JID("iago@shakespeare.lit"));
            initialBlockList.push_back(JID("romeo@montague.net"));
            helperInitialBlockListFetch(initialBlockList);

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), clientBlockListManager_->getBlockList()->getItems().size());
            CPPUNIT_ASSERT_EQUAL(BlockList::Available, clientBlockListManager_->getBlockList()->getState());

            GenericRequest<UnblockPayload>::ref unblockRequest = clientBlockListManager_->createUnblockJIDRequest(JID("romeo@montague.net"));
            unblockRequest->send();
            IQ::ref request = stanzaChannel_->getStanzaAtIndex<IQ>(2);
            CPPUNIT_ASSERT(request.get() != nullptr);
            std::shared_ptr<UnblockPayload> unblockPayload = request->getPayload<UnblockPayload>();
            CPPUNIT_ASSERT(unblockPayload.get() != nullptr);
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.net"), unblockPayload->getItems().at(0));

            IQ::ref unblockRequestResponse = IQ::createResult(request->getFrom(), JID(), request->getID());
            stanzaChannel_->sendIQ(unblockRequestResponse);
            stanzaChannel_->onIQReceived(unblockRequestResponse);

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), clientBlockListManager_->getBlockList()->getItems().size());

            // send block push
            std::shared_ptr<UnblockPayload> pushPayload = std::make_shared<UnblockPayload>();
            pushPayload->addItem(JID("romeo@montague.net"));
            IQ::ref unblockPush = IQ::createRequest(IQ::Set, ownJID_, "push1", pushPayload);
            stanzaChannel_->sendIQ(unblockPush);
            stanzaChannel_->onIQReceived(unblockPush);

            std::vector<JID> blockedJIDs = clientBlockListManager_->getBlockList()->getItems();
            CPPUNIT_ASSERT(blockedJIDs.end() == std::find(blockedJIDs.begin(), blockedJIDs.end(), JID("romeo@montague.net")));
        }

        void testUnblockAllCommand() {
            // start with an already fetched block list
            std::vector<JID> initialBlockList = std::vector<JID>(1, JID("iago@shakespeare.lit"));
            initialBlockList.push_back(JID("romeo@montague.net"));
            initialBlockList.push_back(JID("benvolio@montague.net"));
            helperInitialBlockListFetch(initialBlockList);

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), clientBlockListManager_->getBlockList()->getItems().size());
            CPPUNIT_ASSERT_EQUAL(BlockList::Available, clientBlockListManager_->getBlockList()->getState());

            GenericRequest<UnblockPayload>::ref unblockRequest = clientBlockListManager_->createUnblockAllRequest();
            unblockRequest->send();
            IQ::ref request = stanzaChannel_->getStanzaAtIndex<IQ>(2);
            CPPUNIT_ASSERT(request.get() != nullptr);
            std::shared_ptr<UnblockPayload> unblockPayload = request->getPayload<UnblockPayload>();
            CPPUNIT_ASSERT(unblockPayload.get() != nullptr);
            CPPUNIT_ASSERT_EQUAL(true, unblockPayload->getItems().empty());

            IQ::ref unblockRequestResponse = IQ::createResult(request->getFrom(), JID(), request->getID());
            stanzaChannel_->sendIQ(unblockRequestResponse);
            stanzaChannel_->onIQReceived(unblockRequestResponse);

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), clientBlockListManager_->getBlockList()->getItems().size());

            // send block push
            std::shared_ptr<UnblockPayload> pushPayload = std::make_shared<UnblockPayload>();
            IQ::ref unblockPush = IQ::createRequest(IQ::Set, ownJID_, "push1", pushPayload);
            stanzaChannel_->sendIQ(unblockPush);
            stanzaChannel_->onIQReceived(unblockPush);

            CPPUNIT_ASSERT_EQUAL(true, clientBlockListManager_->getBlockList()->getItems().empty());
        }

        void tearDown() {
            delete clientBlockListManager_;
            delete iqRouter_;
            delete stanzaChannel_;
        }

    private:
        void helperInitialBlockListFetch(const std::vector<JID>& blockedJids) {
            std::shared_ptr<BlockList> blockList = clientBlockListManager_->requestBlockList();
            CPPUNIT_ASSERT(blockList);

            // check for IQ request
            IQ::ref request = stanzaChannel_->getStanzaAtIndex<IQ>(0);
            CPPUNIT_ASSERT(request.get() != nullptr);
            std::shared_ptr<BlockListPayload> requestPayload = request->getPayload<BlockListPayload>();
            CPPUNIT_ASSERT(requestPayload.get() != nullptr);

            CPPUNIT_ASSERT_EQUAL(BlockList::Requesting, blockList->getState());
            CPPUNIT_ASSERT_EQUAL(BlockList::Requesting, clientBlockListManager_->getBlockList()->getState());

            // build IQ response
            std::shared_ptr<BlockListPayload> responsePayload = std::make_shared<BlockListPayload>();
            for (const auto& jid : blockedJids) {
                responsePayload->addItem(jid);
            }

            IQ::ref response = IQ::createResult(ownJID_, JID(), request->getID(), responsePayload);
            stanzaChannel_->sendIQ(response);
            stanzaChannel_->onIQReceived(response);

            CPPUNIT_ASSERT_EQUAL(BlockList::Available, clientBlockListManager_->getBlockList()->getState());
            CPPUNIT_ASSERT(responsePayload->getItems() == clientBlockListManager_->getBlockList()->getItems());
        }


    private:
        JID ownJID_;
        IQRouter* iqRouter_;
        DummyStanzaChannel* stanzaChannel_;
        ClientBlockListManager* clientBlockListManager_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ClientBlockListManagerTest);

