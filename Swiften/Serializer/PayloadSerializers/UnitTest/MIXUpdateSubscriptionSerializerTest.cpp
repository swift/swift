/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Serializer/PayloadSerializers/MIXUpdateSubscriptionSerializer.h>

using namespace Swift;

TEST(MIXUpdateSubscriptionSerializerTest, XEP0369_Example28) {
    MIXUpdateSubscriptionSerializer testling;

    auto update = std::make_shared<MIXUpdateSubscription>();
    update->addSubscription(std::string("urn:xmpp:mix:nodes:messages"));

    std::string expectedResult = "<update-subscription xmlns=\"urn:xmpp:mix:0\">"
                                    "<subscribe node=\"urn:xmpp:mix:nodes:messages\"/>"
                                "</update-subscription>";
    ASSERT_EQ(expectedResult, testling.serialize(update));
}

TEST(MIXUpdateSubscriptionSerializerTest, XEP0369_Example28WithJID) {
    MIXUpdateSubscriptionSerializer testling;

    auto update = std::make_shared<MIXUpdateSubscription>();
    update->setJID(JID("hag66@shakespeare.example"));
    update->addSubscription(std::string("urn:xmpp:mix:nodes:messages"));

    std::string expectedResult = "<update-subscription jid=\"hag66@shakespeare.example\" xmlns=\"urn:xmpp:mix:0\">"
                                    "<subscribe node=\"urn:xmpp:mix:nodes:messages\"/>"
                                "</update-subscription>";
    ASSERT_EQ(expectedResult, testling.serialize(update));
}
