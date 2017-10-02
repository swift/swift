/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Elements/MIXSetNick.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

TEST(MIXSetNickParserTest, WithNick) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<setnick xmlns=\"urn:xmpp:mix:0\"> <nick>thirdwitch</nick> </setnick>"
                ));

    auto payload = parser.getPayload<MIXSetNick>();
    ASSERT_TRUE(payload);
    ASSERT_EQ("thirdwitch", payload->getNick());
}
