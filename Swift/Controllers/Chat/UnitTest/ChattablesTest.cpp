/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <gtest/gtest.h>
#include <hippomocks.h>

#include <Swift/Controllers/Chat/Chattables.h>

// Clang wrongly things that tests for 0 are using 0 as null.
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"

using namespace Swift;

class ChattablesTest : public ::testing::Test {
    protected:
        void SetUp() {
            beginListSize_ = 0;
            endListSize_ = 0;
            callsToBegin_ = 0;
            callsToEnd_ = 0;
        }

        void TearDown() {
        }

        void updateBeginListSize(int listSize);
        void updateEndListSize();

        Chattables chattables_;
        int beginListSize_;
        int endListSize_;
        int callsToBegin_;
        int callsToEnd_;
};

void ChattablesTest::updateBeginListSize(int listSize) {
    beginListSize_ = listSize;
    callsToBegin_++;
}

void ChattablesTest::updateEndListSize() {
    endListSize_ = chattables_.get().size();
    callsToEnd_++;
}

TEST_F(ChattablesTest, testAddJID) {
    chattables_.onBeginAdd.connect([this](int listSize){ updateBeginListSize(listSize); });
    chattables_.onAdded.connect([this](){ updateEndListSize(); });
    JID jid("user@swift.jid");

    chattables_.addJID(jid, Chattables::State::Type::Person);
    ASSERT_EQ(0, beginListSize_);
    ASSERT_EQ(1, endListSize_);
    ASSERT_EQ(1, callsToBegin_);
    ASSERT_EQ(1, callsToEnd_);

    ASSERT_EQ(jid, chattables_.get()[0]);
    Chattables::State state = chattables_.getState(jid);
    ASSERT_EQ(jid, state.jid);
    ASSERT_EQ(Chattables::State::Type::Person, state.type);
}

TEST_F(ChattablesTest, testAddMultipleJIDs) {
    chattables_.onBeginAdd.connect([this](int listSize){ updateBeginListSize(listSize); });
    chattables_.onAdded.connect([this](){ updateEndListSize(); });
    JID jid0("user0@swift.jid");
    JID jid1("user1@swift.jid");
    JID jid2("user2@swift.jid");

    chattables_.addJID(jid0, Chattables::State::Type::Person);
    ASSERT_EQ(0, beginListSize_);
    ASSERT_EQ(1, endListSize_);
    ASSERT_EQ(1, callsToBegin_);
    ASSERT_EQ(1, callsToEnd_);
    chattables_.addJID(jid1, Chattables::State::Type::Person);
    ASSERT_EQ(1, beginListSize_);
    ASSERT_EQ(2, endListSize_);
    ASSERT_EQ(2, callsToBegin_);
    ASSERT_EQ(2, callsToEnd_);
    chattables_.addJID(jid2, Chattables::State::Type::Person);
    ASSERT_EQ(2, beginListSize_);
    ASSERT_EQ(3, endListSize_);
    ASSERT_EQ(3, callsToBegin_);
    ASSERT_EQ(3, callsToEnd_);
}

TEST_F(ChattablesTest, testAddRoomJID) {
    chattables_.onBeginAdd.connect([this](int listSize){ updateBeginListSize(listSize); });
    chattables_.onAdded.connect([this](){ updateEndListSize(); });
    JID jid("room@swift.jid");

    chattables_.addJID(jid, Chattables::State::Type::Room);
    ASSERT_EQ(0, beginListSize_);
    ASSERT_EQ(1, endListSize_);
    ASSERT_EQ(1, callsToBegin_);
    ASSERT_EQ(1, callsToEnd_);

    ASSERT_EQ(jid, chattables_.get()[0]);
    Chattables::State state = chattables_.getState(jid);
    ASSERT_EQ(jid, state.jid);
    ASSERT_EQ(Chattables::State::Type::Room, state.type);
}

TEST_F(ChattablesTest, testSetState) {
    JID jid("user@swift.jid");
    chattables_.addJID(jid, Chattables::State::Type::Person);
    ASSERT_EQ(1, chattables_.get().size());
    ASSERT_EQ(jid, chattables_.get()[0]);
    ASSERT_EQ(Chattables::State::Type::Person, chattables_.getState(jid).type);
    ASSERT_EQ(StatusShow::None, chattables_.getState(jid).status);

    JID returnedJID;
    int returnedIndex;
    int callsToChanged = 0;
    chattables_.onChanged.connect([&returnedJID, &returnedIndex, &callsToChanged](const JID& jid, int index){
        returnedJID = jid;
        returnedIndex = index;
        callsToChanged++;
    });

    Chattables::State newState;
    newState.jid = jid;
    newState.type = Chattables::State::Type::Room;
    newState.status = StatusShow::Online;
    chattables_.setState(jid, newState);

    auto storedState = chattables_.getState(jid);

    ASSERT_EQ(1, chattables_.get().size());
    ASSERT_EQ(jid, chattables_.get()[0]);
    ASSERT_EQ(jid, returnedJID);
    ASSERT_EQ(0, returnedIndex);
    ASSERT_EQ(1, callsToChanged);
    ASSERT_EQ(jid, storedState.jid);
    ASSERT_EQ(Chattables::State::Type::Room, storedState.type);
    ASSERT_EQ(StatusShow::Online, storedState.status);
}
