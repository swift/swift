/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Elements/MIXDestroy.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

TEST(MIXDestroyParserTest, XEP0369_Example70) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<destroy channel=\"coven\" xmlns=\"urn:xmpp:mix:0\"/>"
                ));

    auto payload = parser.getPayload<MIXDestroy>();
    ASSERT_TRUE(payload);

    ASSERT_EQ(std::string("coven"), payload->getChannel());
}
