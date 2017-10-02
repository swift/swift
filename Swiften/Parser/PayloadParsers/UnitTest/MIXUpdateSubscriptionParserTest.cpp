/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Elements/MIXUpdateSubscription.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

TEST(MIXUpdateSubscriptionParserTest, XEP0369_Example28) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<update-subscription xmlns=\"urn:xmpp:mix:0\">"
                    "<subscribe node=\"urn:xmpp:mix:nodes:messages\"/>"
                "</update-subscription>"
                ));

    auto payload = parser.getPayload<MIXUpdateSubscription>();
    ASSERT_TRUE(payload);

    ASSERT_FALSE(payload->getJID());

    auto items = payload->getSubscriptions();
    ASSERT_EQ(static_cast<size_t>(1), items.size());
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:messages")));
}

TEST(MIXUpdateSubscriptionParserTest, XEP0369_Example28_WithJID) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<update-subscription xmlns=\"urn:xmpp:mix:0\" jid=\"hag66@shakespeare.example\">"
                    "<subscribe node=\"urn:xmpp:mix:nodes:messages\"/>"
                "</update-subscription>"
                ));

    auto payload = parser.getPayload<MIXUpdateSubscription>();
    ASSERT_TRUE(payload);

    ASSERT_TRUE(payload->getJID());
    ASSERT_EQ(JID("hag66@shakespeare.example"), *payload->getJID());

    auto items = payload->getSubscriptions();
    ASSERT_EQ(static_cast<size_t>(1), items.size());
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:messages")));
}
