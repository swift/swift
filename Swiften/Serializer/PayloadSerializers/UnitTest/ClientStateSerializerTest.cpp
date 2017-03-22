/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Serializer/PayloadSerializers/ClientStateSerializer.h>

using namespace Swift;

TEST(ClientStateSerializerTest, testSerialize_ActiveState) {
    ClientStateSerializer testling;
    std::shared_ptr<ClientState> priority(new ClientState(ClientState::ClientStateType::Active));

    ASSERT_EQ(std::string("<active xmlns=\"urn:xmpp:csi:0\"/>"), testling.serialize(priority));
}

TEST(ClientStateSerializerTest, testSerialize_InacativeState) {
    ClientStateSerializer testling;
    std::shared_ptr<ClientState> priority(new ClientState(ClientState::ClientStateType::Inactive));

    ASSERT_EQ(std::string("<inactive xmlns=\"urn:xmpp:csi:0\"/>"), testling.serialize(priority));
}
