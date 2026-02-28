/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Elements/MIXLeave.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

TEST(MIXLeaveParserTest, XEP0369_Example33) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse("<leave xmlns=\"urn:xmpp:mix:0\" channel=\"coven@mix.shakespeare.example\"/>"));

    auto payload = parser.getPayload<MIXLeave>();
    ASSERT_TRUE(payload);

    ASSERT_TRUE(payload->getChannel());
    ASSERT_EQ(JID("coven@mix.shakespeare.example"), *payload->getChannel());
}

TEST(MIXLeaveParserTest, XEP0369_Example34) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse("<leave xmlns=\"urn:xmpp:mix:0\"/>"));

    auto payload = parser.getPayload<MIXLeave>();
    ASSERT_TRUE(payload);
    ASSERT_FALSE(payload->getChannel());
}
