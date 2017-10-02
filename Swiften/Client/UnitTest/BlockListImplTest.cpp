/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <vector>

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Client/BlockListImpl.h>
#include <Swiften/JID/JID.h>

using namespace Swift;

class BlockListImplTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(BlockListImplTest);
        CPPUNIT_TEST(testSetItemsToSubset);
        CPPUNIT_TEST(testSetItemsToSuperset);
        CPPUNIT_TEST(testSetItemsAllDifferent);
        CPPUNIT_TEST_SUITE_END();

    public:

        void testSetItemsToSubset() {
            std::vector<JID> subset;
            subset.push_back(JID("a@example.com"));

            blockList_->setItems(subset);

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), addedJIDs_.size());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), removedJIDs_.size());
        }

        void testSetItemsToSuperset() {
            std::vector<JID> superset;
            superset.push_back(JID("a@example.com"));
            superset.push_back(JID("b@example.com"));
            superset.push_back(JID("c@example.com"));

            blockList_->setItems(superset);

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), addedJIDs_.size());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), removedJIDs_.size());
        }

        void testSetItemsAllDifferent() {
            std::vector<JID> newBlockList;
            newBlockList.push_back(JID("x@example.com"));
            newBlockList.push_back(JID("y@example.com"));
            newBlockList.push_back(JID("z@example.com"));

            blockList_->setItems(newBlockList);

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), addedJIDs_.size());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), removedJIDs_.size());
        }

        void setUp() {
            blockList_ = std::make_shared<BlockListImpl>();
            addedJIDs_.clear();
            removedJIDs_.clear();
            blockList_->addItem(JID("a@example.com"));
            blockList_->addItem(JID("b@example.com"));

            blockList_->onItemAdded.connect(boost::bind(&BlockListImplTest::handleBlockListItemAdded, this, _1));
            blockList_->onItemRemoved.connect(boost::bind(&BlockListImplTest::handleBlockListItemRemoved, this, _1));
        }

        void tearDown() {
            blockList_.reset();
        }

        void handleBlockListItemAdded(const JID& jid) {
            addedJIDs_.push_back(jid);
        }

        void handleBlockListItemRemoved(const JID& jid) {
            removedJIDs_.push_back(jid);
        }

    private:
        std::shared_ptr<BlockListImpl> blockList_;
        std::vector<JID> addedJIDs_;
        std::vector<JID> removedJIDs_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(BlockListImplTest);
