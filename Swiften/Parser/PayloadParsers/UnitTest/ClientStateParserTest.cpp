/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Elements/ClientState.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

TEST(ClientStateParserTest, testParse_Active) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<active xmlns='urn:xmpp:csi:0'/>"
                ));

    ClientState::ref payload = parser.getPayload<ClientState>();
    ASSERT_EQ(ClientState::ClientStateType::Active, payload->getClientState());
}

TEST(ClientStateParserTest, testParse_Inactive) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<inactive xmlns='urn:xmpp:csi:0'/>"
                ));

    ClientState::ref payload = parser.getPayload<ClientState>();
    ASSERT_EQ(ClientState::ClientStateType::Inactive, payload->getClientState());
}
