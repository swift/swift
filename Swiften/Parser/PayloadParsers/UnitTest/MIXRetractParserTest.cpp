/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Elements/MIXRetract.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

TEST(MIXRetractParserTest, XEP0369_Example22) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse("<retract id=\"28482-98726-73623\" xmlns=\"urn:xmpp:mix:0\"/>"));

    auto payload = parser.getPayload<MIXRetract>();
    ASSERT_TRUE(payload);
    ASSERT_EQ(std::string("28482-98726-73623"), payload->getMessageID());
}
