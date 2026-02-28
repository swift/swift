/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Elements/MIXRegisterNick.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

TEST(MIXRegisterNickParserTest, WithNick) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<register xmlns=\"urn:xmpp:mix:0\"> <nick>thirdwitch</nick> </register>"
                ));

    auto payload = parser.getPayload<MIXRegisterNick>();
    ASSERT_TRUE(payload);
    ASSERT_EQ("thirdwitch", payload->getNick());
}
